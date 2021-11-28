#include "parser.h"
#include "ppm.h"
#include "outsidehelper.h"
#include "tinyxml2.h"
#include "externScene.h" // to extern created_scene
#include <sstream>
#include <stdexcept>

#include <limits>  //
#include <thread>  // Included by me
#include <mutex>   //
#define ONE_MILLIONTH 1e-6
#define NO_INTERSECTION {DEFINED_INFINITY,{},-1}

const float DEFINED_INFINITY = std::numeric_limits<float>::max();


void parser::Scene::loadFromXml(const std::string &filepath)
{
    tinyxml2::XMLDocument file;
    std::stringstream stream;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> background_color.x >> background_color.y >> background_color.z;

    //Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        stream >> camera.near_plane.x >> camera.near_plane.y >> camera.near_plane.z >> camera.near_plane.w;
        stream >> camera.near_distance;
        stream >> camera.image_width >> camera.image_height;
        stream >> camera.image_name;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

    //Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
    element = element->FirstChildElement("PointLight");
    PointLight point_light;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
        stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        material.is_mirror = (element->Attribute("type", "mirror") != NULL);

        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("MirrorReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phong_exponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    //Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertex_data.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.material_id;

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0_id).eof())
        {
            stream >> face.v1_id >> face.v2_id;
            mesh.faces.push_back(face);
        }
        stream.clear();

        meshes.push_back(mesh);
        mesh.faces.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    Triangle triangle;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> triangle.material_id;

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

        triangles.push_back(triangle);
        element = element->NextSiblingElement("Triangle");
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere sphere;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> sphere.material_id;

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> sphere.center_vertex_id;

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}


int lastRow;
std::mutex rowMutex;

// INTERSECTION FUNCTIONS -----------------------------------------------------------------------------------

    // General ray intersection function
parser::IntersectionData intersect_obj(parser::Ray ray, std::vector<parser::Sphere> spheres, std::vector<parser::Triangle> triangles, std::vector<parser::Mesh> meshes) {
    
    parser::IntersectionData closest_intersection = { DEFINED_INFINITY, {}, -1 };
    // Since all objects are not together, this block iterate all objects (spheres, triangles, and meshes, respectively) 
    // and then sets the value of closest_intersection. At each for loop object calls its own intersection method.
    for (int i = 0; i < spheres.size(); i++) {
        parser::IntersectionData current_intersection = spheres[i].intersect(ray);
        if (current_intersection.t != DEFINED_INFINITY) {
            if (current_intersection.t < closest_intersection.t) {
                closest_intersection = current_intersection;
            } else;
        } else;
    }
    for (int i = 0; i < triangles.size(); i++) {
        parser::IntersectionData current_intersection = triangles[i].intersect(ray);
        if (current_intersection.t != DEFINED_INFINITY) {
            if (current_intersection.t < closest_intersection.t) {
                closest_intersection = current_intersection;
            } else;
        } else;
    }
    for (int i = 0; i < meshes.size(); i++) {
        parser::IntersectionData current_intersection = meshes[i].intersect(ray);
        if (current_intersection.t != DEFINED_INFINITY) {
            if (current_intersection.t < closest_intersection.t) {
                closest_intersection = current_intersection;
            } else;
        } else;
    }
    return closest_intersection;
}

    // Sphere ray intersection function
parser::IntersectionData parser::Sphere::intersect(const Ray & ray) {
    // Get center point of sphere
    Vec3f center_of_sphere = created_scene.vertex_data[center_vertex_id-1];

    float b = dot_product(ray.direction, vector_subtract(ray.origin, center_of_sphere));
    float c = dot_product(vector_subtract(ray.origin, center_of_sphere), vector_subtract(ray.origin, center_of_sphere)) - (radius * radius);
    float dscrmnt_val = (b*b) - c;

    if(dscrmnt_val < ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;

    float t1 = -b - sqrt(dscrmnt_val); // sqrt(x) performance better than pow(x, 0.5)
    float t2 = -b + sqrt(dscrmnt_val);

    if(t1 < ONE_MILLIONTH && t2 < ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;

    return { t1, normalize(vector_subtract(vector_sum(ray.origin, vector_multipleS(t1, ray.direction)), center_of_sphere)), material_id };
}

    // Triangle ray intersection function
parser::IntersectionData parser::Triangle::intersect(const Ray & ray) {

    Vec3f p1 = created_scene.vertex_data[indices.v0_id-1];
    Vec3f p2 = created_scene.vertex_data[indices.v1_id-1];
    Vec3f p3 = created_scene.vertex_data[indices.v2_id-1];

    float det = calc_determinant(
            p1.x - p2.x, p1.x - p3.x, ray.direction.x,
            p1.y - p2.y, p1.y - p3.y, ray.direction.y,
            p1.z - p2.z, p1.z - p3.z, ray.direction.z);

    if(det < ONE_MILLIONTH && det > -ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;
        

    float beta = calc_determinant(
            p1.x - ray.origin.x, p1.x - p3.x, ray.direction.x,
            p1.y - ray.origin.y, p1.y - p3.y, ray.direction.y,
            p1.z - ray.origin.z, p1.z - p3.z, ray.direction.z)
                 / det;
    float gamma = calc_determinant(
            p1.x - p2.x, p1.x - ray.origin.x, ray.direction.x,
            p1.y - p2.y, p1.y - ray.origin.y, ray.direction.y,
            p1.z - p2.z, p1.z - ray.origin.z, ray.direction.z)
                 / det;
    float t = calc_determinant(
            p1.x - p2.x, p1.x - p3.x, p1.x - ray.origin.x,
            p1.y - p2.y, p1.y - p3.y, p1.y - ray.origin.y,
            p1.z - p2.z, p1.z - p3.z, p1.z - ray.origin.z)
              / det;

    if (t > ONE_MILLIONTH && beta + gamma <= 1 && 0 <= beta && 0 <= gamma) {
        return {t, normalize(cross_product(vector_subtract(p3, p2), vector_subtract(p1, p2))), material_id};
    } else;

    return NO_INTERSECTION;
}

    // This is for Mesh faces. Same as the triangle one above.
parser::IntersectionData parser::Face::intersect(const Ray & ray, int material_id) {

    Vec3f p1 = created_scene.vertex_data[v0_id-1];
    Vec3f p2 = created_scene.vertex_data[v1_id-1];
    Vec3f p3 = created_scene.vertex_data[v2_id-1];

    float det = calc_determinant(
            p1.x - p2.x, p1.x - p3.x, ray.direction.x,
            p1.y - p2.y, p1.y - p3.y, ray.direction.y,
            p1.z - p2.z, p1.z - p3.z, ray.direction.z);

    if(det < ONE_MILLIONTH && det > -ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;
        
    float beta = calc_determinant(
            p1.x - ray.origin.x, p1.x - p3.x, ray.direction.x,
            p1.y - ray.origin.y, p1.y - p3.y, ray.direction.y,
            p1.z - ray.origin.z, p1.z - p3.z, ray.direction.z)
                 / det;
    float gamma = calc_determinant(
            p1.x - p2.x, p1.x - ray.origin.x, ray.direction.x,
            p1.y - p2.y, p1.y - ray.origin.y, ray.direction.y,
            p1.z - p2.z, p1.z - ray.origin.z, ray.direction.z)
                 / det;
    float t = calc_determinant(
            p1.x - p2.x, p1.x - p3.x, p1.x - ray.origin.x,
            p1.y - p2.y, p1.y - p3.y, p1.y - ray.origin.y,
            p1.z - p2.z, p1.z - p3.z, p1.z - ray.origin.z)
              / det;

    if (t > ONE_MILLIONTH && beta + gamma <= 1 && 0 <= beta && 0 <= gamma) {
        return {t, normalize(cross_product(vector_subtract(p3, p2), vector_subtract(p1, p2))), material_id};
    } else;
        
    return NO_INTERSECTION;
}

    // Mesh ray intersection function
parser::IntersectionData parser::Mesh::intersect(const Ray & ray) {

    int size = faces.size();
    IntersectionData tempMin = NO_INTERSECTION;

    for(int i=0; i < size; i++) {
        IntersectionData inters = faces[i].intersect(ray, material_id);
        if(inters.t < tempMin.t) {
            tempMin = inters;
        } else;
    }
    return tempMin;
}

parser::Vec3f calc_specular(const parser::Material * material, const parser::Vec3f & normalVector, const parser::Vec3f & irradiance, const parser::Vec3f & halfVector) {
    // (cosAlpha)^ns
    float phongExponentCosAlpha = std::pow(std::max(0.0f, dot_product(normalVector, halfVector)), material->phong_exponent);
    // (cosAlpha)^ns * E(d)
    parser::Vec3f specular = vector_multipleS(phongExponentCosAlpha, irradiance);
    // Multiplying with specular coeff
    specular.r *= material->specular.r;
    specular.g *= material->specular.g;
    specular.b *= material->specular.b;
    return specular;
}

parser::Vec3f calc_diffuse(const parser::Material * material, const parser::Vec3f & normalVector,
    const parser::Vec3f & normalizedLightDirection, const parser::Vec3f & irradiance) {
    // cosTheta
    float cosTheta = std::max(0.0f, dot_product(normalizedLightDirection, normalVector));
    // cosTheta * E(d)
    parser::Vec3f diffuse = vector_multipleS(cosTheta, irradiance);
    // Multiplying with diffuse coeff
    diffuse.r *= material->diffuse.r;
    diffuse.g *= material->diffuse.g;
    diffuse.b *= material->diffuse.b;
    return diffuse;
}

parser::Vec3f calc_ambient(const parser::Material * material, const parser::Vec3f & ambientLight) {
    return { material->ambient.r * ambientLight.r, material->ambient.g * ambientLight.g, material->ambient.b * ambientLight.b};
}

parser::Vec3f calc_radiance(const parser::Ray & ray, const parser::IntersectionData & intersection, parser::Scene * scene, int remainingRecursion) {
    
    parser::Vec3f pixelColor = {};
    parser::Material * intersectionMaterial = &scene->materials[intersection.materialId - 1];

    // Calculate Ambient shading and add it to pixelColor (adding ambient directly to all pixels)
    parser::Vec3f ambientContribution = calc_ambient(intersectionMaterial, scene->ambient_light);
    pixelColor = ambientContribution;

    parser::Vec3f intersectionPoint = vector_sum(ray.origin, vector_multipleS(intersection.t, ray.direction));
    // subtract intPoint from camera's position (origin) and find the vector that goes to eye
    parser::Vec3f eyeVector =  vector_subtract(ray.origin, intersectionPoint); //  w_0
    parser::Vec3f normalizedEyeVector = normalize(eyeVector);

    for (int i = 0; i < scene->point_lights.size(); ++i) {
        parser::Vec3f lightDirection = vector_subtract(scene->point_lights[i].position, intersectionPoint); // w_i
        parser::Vec3f normalizedLightDirection = normalize(lightDirection);

        // Generate the shadow ray s from intersection point to i
        parser::Ray shadowRay;
        parser::Vec3f intOffset = vector_multipleS(scene->shadow_ray_epsilon, normalizedLightDirection);
        shadowRay.origin = vector_sum(intersectionPoint, intOffset);
        shadowRay.direction = normalizedLightDirection;

        // Intersect s with all objects again to check if there is any obj between the light source and point
        parser::IntersectionData shadowIntersection = intersect_obj(shadowRay, scene->spheres, scene->triangles, scene->meshes);

        // If there is an intersection continue -- point is in shadow -- no contribution from this light
        // Else calculate diffuse and specular shading from this light source and add them to the pixelColor -- there is contribution from this light source -- point is not in shadow
        if (shadowIntersection.t >= vec_length(lightDirection)) {
            parser::Vec3f irradiance = scene->point_lights[i].computeLightContribution(intersectionPoint);

            // Compute Diffuse
            parser::Vec3f diffuseContribution = calc_diffuse(intersectionMaterial, intersection.normal, normalizedLightDirection, irradiance);
            pixelColor.r += diffuseContribution.r;
            pixelColor.g += diffuseContribution.g;
            pixelColor.b += diffuseContribution.b;

            // Compute Specular
            parser::Vec3f normalizedHalfVector = normalize(vector_sum(normalizedLightDirection, normalizedEyeVector));
            parser::Vec3f specularContribution = calc_specular(intersectionMaterial, intersection.normal, irradiance, normalizedHalfVector);
            pixelColor.r += specularContribution.r;
            pixelColor.g += specularContribution.g;
            pixelColor.b += specularContribution.b;
        } else {
            // Else there is an intersection continue -- point is in shadow -- no contribution from this light
            continue;
        }
    }

    // Check if the material of intersected object has a nonzero MirrorReflectance value
    // Then Bounce primary ray until no intersection or maxRecDepth (count is initially zero)

    if ((intersectionMaterial->mirror.x > 0 || intersectionMaterial->mirror.y > 0 ||
    intersectionMaterial->mirror.z > 0) && remainingRecursion > 0) {

        // Calculate reflected ray's direction using w_r = -w_0 + 2*n*cosTheta => cosTheta = n.w_0
        // Also move set its origin as intersectionPoint which is moved a bit further by shadowRayEps
        parser::Ray reflectedRay;
        float cosTheta = dot_product(intersection.normal, normalizedEyeVector);
        reflectedRay.direction = vector_sum(vector_multipleS(-1, normalizedEyeVector), vector_multipleS(2 * cosTheta, intersection.normal));
        reflectedRay.origin = vector_sum(intersectionPoint, vector_multipleS(scene->shadow_ray_epsilon, reflectedRay.direction));

        // Again Calculate the nearest intersection of reflected Ray
        parser::IntersectionData reflectedIntersection = intersect_obj(reflectedRay, scene->spheres, scene->triangles, scene->meshes);

        if (reflectedIntersection.t != DEFINED_INFINITY) { // means that ray hit an object
            parser::Vec3f reflectedRadiance = calc_radiance(reflectedRay, reflectedIntersection, scene, remainingRecursion-1);

            reflectedRadiance.x *= intersectionMaterial->mirror.x;
            reflectedRadiance.y *= intersectionMaterial->mirror.y;
            reflectedRadiance.z *= intersectionMaterial->mirror.z;
            pixelColor.r += reflectedRadiance.r;
            pixelColor.g += reflectedRadiance.g;
            pixelColor.b += reflectedRadiance.b;
        }
    }

    // Dont forget to clamp the resulting pixelColor
    pixelColor.r = std::min(std::max(0.0f, pixelColor.r), 255.0f);
    pixelColor.g = std::min(std::max(0.0f, pixelColor.g), 255.0f);
    pixelColor.b = std::min(std::max(0.0f, pixelColor.b), 255.0f);

    // TODO: Handle reflectance of pixel

    return pixelColor;
}

parser::Vec3f converter(parser::Vec3i a) {
    parser::Vec3f result;
    result.r = a.x;
    result.g = a.y;
    result.b = a.z;
    return result;
}

parser::Color renderPixel(int col, int row, parser::Scene * scene, int camIndex) {
    // Calculate primary ray from Camera x that goes through pixel
    parser::Ray primRay = scene->cameras[camIndex].getPrimaryRay(row, col);

    // Calculate nearest intersection
    parser::IntersectionData intersection = intersect_obj(primRay, scene->spheres, scene->triangles, scene->meshes);

    if (intersection.t != DEFINED_INFINITY) { // means that ray hit an object
        parser::Vec3f pxColor = calc_radiance(primRay, intersection, scene, scene->max_recursion_depth);
        return {static_cast<unsigned char>(pxColor.r),
                static_cast<unsigned char>(pxColor.g),
                static_cast<unsigned char>(pxColor.b)};
    }
    else { // no intersection, just set the pixel's color to background color
    parser::Vec3f temp = converter(scene->background_color);
        return {static_cast<unsigned char>(temp.r),
                static_cast<unsigned char>(temp.g),
                static_cast<unsigned char>(temp.b)};
    }
}

void renderRow(parser::Image * image, const int row, parser::Scene * scene, int camIndex) {
    for (int col = 0; col < scene->cameras[camIndex].image_width; ++col) {
        parser::Color colorOfPixel = renderPixel(col, row, scene, camIndex);
        image->setPixelValue(col, row, colorOfPixel, image->width);
    }
}

int getTask() {
    std::lock_guard<std::mutex> guard(rowMutex);
    --lastRow;
    return lastRow;
}

void execute(parser::Image * image, parser::Scene * scene, int camIndex) {
    while (true) {
        int rowNum = getTask();
        if (rowNum < 0)
            break;
        renderRow(image, rowNum, scene, camIndex);

    }
}

parser::Ray parser::Camera::getPrimaryRay(int row, int col) {
    Vec3f origin = position; // e
    Vec3f imageCenter = vector_sum(origin, vector_multipleS(near_distance, gaze)); // m
    Vec3f topLeft = vector_sum(vector_sum(imageCenter, vector_multipleS(near_plane.x, right)), vector_multipleS(near_plane.w, up)); // q
    float i = (near_plane.y - near_plane.x) * (col + 0.5) / image_width; // s_u
    float j = (near_plane.w - near_plane.z) * (row + 0.5) / image_height; // s_v
    Vec3f targetPoint = vector_sum(topLeft, vector_subtract(vector_multipleS(i, right), vector_multipleS(j, up))); // s
    // We have to normalize the direction to the length of 1 so it doesn't skew our results
    Vec3f rayDirection = normalize(vector_subtract(targetPoint, origin)); // d = s - e
    Ray ray = Ray(position, rayDirection);
    return ray;
}

parser::Vec3f parser::PointLight::computeLightContribution(const Vec3f& p) {
    Vec3f lightDirection = vector_subtract(position, p);
    float lightDistance = vec_length(lightDirection);
    Vec3f irradianceContribution =  vector_division((lightDistance * lightDistance), intensity);
    return irradianceContribution;
}

parser::Vec3f parser::vector_sum(Vec3f first, Vec3f second) {
    Vec3f res;
    res.x = first.x + second.x;
    res.y = first.y + second.y;
    res.z = first.z + second.z;
	return res;
}

parser::Vec3f parser::vector_subtract(Vec3f first, Vec3f second) {
    Vec3f res;
    res.x = first.x - second.x;
    res.y = first.y - second.y;
    res.z = first.z - second.z;
	return res;
}

parser::Vec3f parser::vector_multipleS(float scale, Vec3f vector) {
    Vec3f res;
    res.x = scale * vector.x;
    res.y = scale * vector.y;
    res.z = scale * vector.z;
	return res;
}

parser::Vec3f parser::vector_division(float scale, Vec3f & vector) {
    Vec3f res;
    res.x = vector.x / scale;
    res.y = vector.y / scale;
    res.z = vector.z / scale;
    return res;
}

parser::Vec3f parser::Ray::getPoint(float t) {
    return vector_sum(origin, vector_multipleS(t, direction));
}

float parser::Ray::gett(const Vec3f & p) {
    Vec3f diff = vector_subtract(p, origin);
    return (diff.x + diff.y + diff.z)  / (direction.x + direction.y + direction.z);
}

// Image related functions ----------------------------------------------------------

parser::Image::Image(int width, int height) : width(width), height(height) {
    data = new unsigned char [height * width * 3];
}

//
// Set the value of the pixel at the given column and row
//
void parser::Image::setPixelValue(int col, int row, Color &color, int width) {
    data[row*width*3 + 3*col] = color.channel[0];
    data[row*width*3 + 3*col+1] = color.channel[1];
    data[row*width*3 + 3*col+2] = color.channel[2];
}

void parser::Scene::renderScene(void) {

    const unsigned int numOfCores = std::thread::hardware_concurrency();
    for (int x = 0; x < cameras.size(); ++x) {
        cameras[x].right = cross_product(cameras[x].gaze, cameras[x].up);
        auto * image = new Image(cameras[x].image_width,cameras[x].image_height);
        lastRow = cameras[x].image_height;
        if (!numOfCores)
            execute(image, this, x);
        else {
            auto * threads = new std::thread[numOfCores];
            for (int i = 0; i < numOfCores; i++) {
                threads[i] = std::thread(execute, image, this, x);
            }
            for (int i = 0; i < numOfCores; i++)
                threads[i].join();
            delete[] threads;
        }

        int n = cameras[x].image_name.length();
        char file_name[n + 1];
        strcpy(file_name, cameras[x].image_name.c_str());

        write_ppm(file_name, image->data, cameras[x].image_width, cameras[x].image_height);
    }
}

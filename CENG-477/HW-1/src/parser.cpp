#include "parser.h"
#include "ppm.h"
#include "tinyxml2.h"
#include <sstream>
#include <stdexcept>
#include "outsidehelper.h" // vector product, normalize, length... 
#include "externScene.h"   // to extern created_scene

#include <limits>  //
#include <thread>  // Included by me
#define ONE_MILLIONTH 1e-6
#define NO_INTERSECTION { DEFINED_INFINITY, {}, -1 }

// Max float number is assigned as infinity to use in intersection functions
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

    //Get shadow_rayEpsilon
    element = root->FirstChildElement("shadow_rayEpsilon");
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

// INTERSECTION FUNCTIONS -----------------------------------------------------------------------------------
    // General ray intersection function
parser::Return_Intersection intersect_obj(parser::Ray &ray, std::vector<parser::Sphere> &spheres, std::vector<parser::Triangle> &triangles, std::vector<parser::Mesh> &meshes) {
    // The closest intersection Return_Intersection info of this ray. Defined at infinity at the beginning
    parser::Return_Intersection closest_intersection = NO_INTERSECTION;
    // Return_Intersection about all intersections generated in for loops
    parser::Return_Intersection current_intersection;
    // Since all objects are not together, this block iterate all objects (spheres, triangles, and meshes, respectively) one by one
    // and then sets the value of closest_intersection. At each for loop, object calls its own intersection method.

    // Spheres
    for (int i = 0; i < spheres.size(); i++) {
        current_intersection = spheres[i].intersect(ray);
        if (current_intersection.t < closest_intersection.t) {
            closest_intersection = current_intersection;
        } else;
    }
    // Triangles
    for (int i = 0; i < triangles.size(); i++) {
        current_intersection = triangles[i].intersect(ray);
        if (current_intersection.t < closest_intersection.t) {
            closest_intersection = current_intersection;
        } else;
    }
    // Meshes
    for (int i = 0; i < meshes.size(); i++) {
        current_intersection = meshes[i].intersect(ray);
        if (current_intersection.t < closest_intersection.t) {
            closest_intersection = current_intersection;
        } else;
    }
    return closest_intersection;
}

    // Sphere ray intersection function
parser::Return_Intersection parser::Sphere::intersect(parser::Ray &ray) {
    // Declaring intersection
    parser::Return_Intersection intersection_point;
    // Get center point of sphere
    Vec3f center_of_sphere = created_scene.vertex_data[center_vertex_id-1];
    Vec3f distance_centers = vector_subtract(ray.origin, center_of_sphere);

    float b = dot_product(ray.direction, distance_centers);
    float c = dot_product(vector_subtract(ray.origin, center_of_sphere), distance_centers) - pow(radius, 2);
    // Calculate discriminant
    float dscrmnt_val = pow(b, 2) - c;

    float t1 = -b - sqrt(dscrmnt_val);
    float t2 = -b + sqrt(dscrmnt_val);

    // Determine whether there is a rot or not || check t1-t2 condition
    if(dscrmnt_val < ONE_MILLIONTH || (t1 < ONE_MILLIONTH && t2 < ONE_MILLIONTH)) {
        return NO_INTERSECTION;
    } else;
    intersection_point.t = t1;
    intersection_point.materialId = material_id;
    intersection_point.normal = normalize(vector_subtract(vector_sum(ray.origin, vector_multipleS(t1, ray.direction)), center_of_sphere));
    return intersection_point;
}

    // Triangle ray intersection function
parser::Return_Intersection parser::Triangle::intersect(parser::Ray &ray) {
    // Declaring intersection
    parser::Return_Intersection intersection_point;
    // Get vertices
    Vec3f vec_v0 = created_scene.vertex_data[indices.v0_id-1];
    Vec3f vec_v1 = created_scene.vertex_data[indices.v1_id-1];
    Vec3f vec_v2 = created_scene.vertex_data[indices.v2_id-1];
    // Variables to use in determinant function
    Vec3f p1_minus_p2 = vector_subtract(vec_v0, vec_v1);
    Vec3f p1_minus_p3 = vector_subtract(vec_v0, vec_v2);
    Vec3f p1_minus_origin = vector_subtract(vec_v0, ray.origin);

    float dtrmnt_val = calc_determinant(p1_minus_p2, p1_minus_p3, ray.direction);

    if(dtrmnt_val < ONE_MILLIONTH && dtrmnt_val > -ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;
        
    // TODO: Make that determinant function great again!!
    float beta = calc_determinant(p1_minus_origin, p1_minus_p3, ray.direction) / dtrmnt_val;
    float gamma = calc_determinant(p1_minus_p2, p1_minus_origin, ray.direction) / dtrmnt_val;
    float t = calc_determinant(p1_minus_p2, p1_minus_p3, p1_minus_origin) / dtrmnt_val;

    if (t > ONE_MILLIONTH && 0 <= gamma && 0 <= beta && beta + gamma <= 1) {
        intersection_point.t = t;
        intersection_point.materialId = material_id;
        intersection_point.normal = normalize(cross_product(vector_subtract(vec_v2, vec_v1), vector_subtract(vec_v0, vec_v1)));
        return intersection_point;
    } else;
    return NO_INTERSECTION;
}

    // This is for Mesh faces. Completely same as the triangle one above.
parser::Return_Intersection parser::Face::intersect(parser::Ray &ray, int material_id) {
    // Declaring intersection
    parser::Return_Intersection intersection_point;
    // Get vertices
    Vec3f vec_v0 = created_scene.vertex_data[v0_id-1];
    Vec3f vec_v1 = created_scene.vertex_data[v1_id-1];
    Vec3f vec_v2 = created_scene.vertex_data[v2_id-1];
    // Variables to use in determinant function
    Vec3f p1_minus_p2 = vector_subtract(vec_v0, vec_v1);
    Vec3f p1_minus_p3 = vector_subtract(vec_v0, vec_v2);
    Vec3f p1_minus_origin = vector_subtract(vec_v0, ray.origin);
    // Calculating determinant
    float dtrmnt_val = calc_determinant(p1_minus_p2, p1_minus_p3, ray.direction);

    if(dtrmnt_val < ONE_MILLIONTH && dtrmnt_val > -ONE_MILLIONTH) {
        return NO_INTERSECTION;
    } else;
        
    // TODO: Make that determinant function great again!!
    float beta = calc_determinant(p1_minus_origin, p1_minus_p3, ray.direction) / dtrmnt_val;
    float gamma = calc_determinant(p1_minus_p2, p1_minus_origin, ray.direction) / dtrmnt_val;
    float t = calc_determinant(p1_minus_p2, p1_minus_p3, p1_minus_origin) / dtrmnt_val;

    if (t > ONE_MILLIONTH && 0 <= gamma && 0 <= beta && beta + gamma <= 1) {
        intersection_point.t = t;
        intersection_point.materialId = material_id;
        intersection_point.normal = normalize(cross_product(vector_subtract(vec_v2, vec_v1), vector_subtract(vec_v0, vec_v1)));
        return intersection_point;
    } else;
    return NO_INTERSECTION;
}

    // Mesh ray intersection function
parser::Return_Intersection parser::Mesh::intersect(Ray & ray) {
    int size = faces.size();
    Return_Intersection current_min_inters = NO_INTERSECTION;

    for(int i=0; i < size; i++) {
        Return_Intersection closest_intrsct = faces[i].intersect(ray, material_id);
        if(closest_intrsct.t < current_min_inters.t) {
            current_min_inters = closest_intrsct;
        } else;
    }
    return current_min_inters;
}

// Calculates ambient contribution
parser::Vec3f calc_ambient(parser::Material &material, parser::Vec3f &ambient_light) {
    return { material.ambient.x * ambient_light.x, material.ambient.y * ambient_light.y, material.ambient.z * ambient_light.z};
}

// Calculates light contribution
parser::Vec3f parser::PointLight::calc_light_contribution(Vec3f &p) {
    return vector_division((std::pow(vec_length(vector_subtract(position, p)), 2)), intensity);;
}

parser::Vec3f calc_radiance(parser::Scene *scene, parser::Ray &ray, parser::Return_Intersection &intersection, int remainingRecursion) {
    // Get material struct from scene->materials
    parser::Material obj_material = scene->materials[intersection.materialId - 1];
    // pixel_color += calc_ambient(x) (Since pixe color is empty, I directly asssigned it.)
    parser::Vec3f pixel_color = calc_ambient(obj_material, scene->ambient_light);
    parser::Vec3f intersection_point = vector_sum(ray.origin, vector_multipleS(intersection.t, ray.direction));
    // Generating normalized eye vector -> normalize(eye_vector)
    parser::Vec3f normalized_eye_vec = normalize(vector_subtract(ray.origin, intersection_point));

    // Iterate through all light in the scene
    for (int i = 0; i < scene->point_lights.size(); ++i) {
        // Calculating light directions
        parser::Vec3f light_drct = vector_subtract(scene->point_lights[i].position, intersection_point);
        parser::Vec3f ligth_drctn_normlzd = normalize(light_drct);
        // Generate shadow rays
        parser::Vec3f int_offset = vector_multipleS(scene->shadow_ray_epsilon, ligth_drctn_normlzd);
        parser::Ray shadow_ray = parser::Ray(vector_sum(intersection_point, int_offset), ligth_drctn_normlzd);

        // if point not in shadow -> add dffs_contribute + spclr_contribute to pixel || else an object between light and point -> no contribution
        if (intersect_obj(shadow_ray, scene->spheres, scene->triangles, scene->meshes).t >= vec_length(light_drct)) {
            // Add light's contribution to pixel
            parser::Vec3f irradiance = scene->point_lights[i].calc_light_contribution(intersection_point);
            // Calculate dffs_contribute and add to pixel_color
            parser::Vec3f dffs_contribute = vector_multipleS(std::max(0.0f, dot_product(ligth_drctn_normlzd, intersection.normal)), irradiance);
            pixel_color.x += dffs_contribute.x *= obj_material.diffuse.x;
            pixel_color.y += dffs_contribute.y *= obj_material.diffuse.y;
            pixel_color.z += dffs_contribute.z *= obj_material.diffuse.z;
            // Calculate spclr_contribute and add to pixel_color
            parser::Vec3f normalized_half_vec = normalize(vector_sum(ligth_drctn_normlzd, normalized_eye_vec));
            parser::Vec3f spclr_contribute = vector_multipleS(std::pow(std::max(0.0f, dot_product(intersection.normal, normalized_half_vec)), obj_material.phong_exponent), irradiance);
            pixel_color.x += spclr_contribute.x *= obj_material.specular.x;
            pixel_color.y += spclr_contribute.y *= obj_material.specular.y;
            pixel_color.z += spclr_contribute.z *= obj_material.specular.z;
        } else;
    }

    if ((obj_material.mirror.x > 0 || obj_material.mirror.y > 0 || obj_material.mirror.z > 0) && remainingRecursion > 0) {
        parser::Ray reflectedRay;
        reflectedRay.direction = vector_sum(vector_multipleS(-1, normalized_eye_vec), vector_multipleS(2 * dot_product(intersection.normal, normalized_eye_vec), intersection.normal));
        reflectedRay.origin = vector_sum(intersection_point, vector_multipleS(scene->shadow_ray_epsilon, reflectedRay.direction));
        parser::Return_Intersection reflection_intersect = intersect_obj(reflectedRay, scene->spheres, scene->triangles, scene->meshes);

        if (reflection_intersect.t != DEFINED_INFINITY) {
            parser::Vec3f reflection_radiance = calc_radiance(scene, reflectedRay, reflection_intersect, remainingRecursion - 1);
            pixel_color.x += reflection_radiance.x *= obj_material.mirror.x;
            pixel_color.y += reflection_radiance.y *= obj_material.mirror.y;
            pixel_color.z += reflection_radiance.z *= obj_material.mirror.z;
        } else;
    } else;
    pixel_color.x = std::min(std::max(0.0f, pixel_color.x), 255.0f);
    pixel_color.y = std::min(std::max(0.0f, pixel_color.y), 255.0f);
    pixel_color.z = std::min(std::max(0.0f, pixel_color.z), 255.0f);
    return pixel_color;
}

// Converting Vec3i into Vec3f || Vec3i<scene->background_color> --> Vec3f<scene->background_color>
parser::Vec3f converter(parser::Vec3i a) {
    parser::Vec3f result_vec;
    result_vec.x = a.x;
    result_vec.y = a.y;
    result_vec.z = a.z;
    return result_vec;
}

// Generating each pixel on image
parser::Color pixel_render(int col, int row, parser::Scene * scene, int camera_indice) {
    // Generating a specific primary ray for current camera
    parser::Ray primRay = scene->cameras[camera_indice].generate_prim_ray(row, col);
    // Returns intersection data (t value, normal vector, and material_id)
    parser::Return_Intersection intersection = intersect_obj(primRay, scene->spheres, scene->triangles, scene->meshes);
    // if -> There is a determined t value (the ray hit an object) -> render pixel; else -> no intersection -> render background_color;
    if (intersection.t != DEFINED_INFINITY) {
        parser::Vec3f pixel_color = calc_radiance(scene, primRay, intersection, scene->max_recursion_depth);
        return { (unsigned char)(pixel_color.x), (unsigned char)(pixel_color.y), (unsigned char)(pixel_color.z)};
    } else {
        parser::Vec3f temp = converter(scene->background_color);
        return { (unsigned char)(temp.x), (unsigned char)(temp.y), (unsigned char)(temp.z) };
    }
}

// Generate primary rays emitted from camera
parser::Ray parser::Camera::generate_prim_ray(int row, int col) {
    // Image middle + near_plane top-left info
    Vec3f top_left_corner = vector_sum(vector_sum(vector_sum(position, vector_multipleS(near_distance, gaze)), vector_multipleS(near_plane.x, right)), vector_multipleS(near_plane.w, up));
    // su & sv variables
    float i = (near_plane.y - near_plane.x) * (col + 0.5) / image_width;
    float j = (near_plane.w - near_plane.z) * (row + 0.5) / image_height;
    // Target - Position, then normalize 
    Vec3f ray_drct = normalize(vector_subtract(vector_sum(top_left_corner, vector_subtract(vector_multipleS(i, right), vector_multipleS(j, up))), position));
    // Generated primary ray
    Ray ray = Ray(position, ray_drct);
    return ray;
}

// VECTOR (Vec3f) RELATED CALCULATION FUNCTIONS --------------------------
parser::Vec3f parser::vector_sum(Vec3f first, Vec3f second) {
    Vec3f result_vec;
    result_vec.x = first.x + second.x;
    result_vec.y = first.y + second.y;
    result_vec.z = first.z + second.z;
	return result_vec;
}

parser::Vec3f parser::vector_subtract(Vec3f first, Vec3f second) {
    Vec3f result_vec;
    result_vec.x = first.x - second.x;
    result_vec.y = first.y - second.y;
    result_vec.z = first.z - second.z;
	return result_vec;
}
    // TODO: replace float and Vec3f arguments of vector_multipleS and vector_division for better understanding
parser::Vec3f parser::vector_multipleS(float scale, Vec3f vector) {
    Vec3f result_vec;
    result_vec.x = scale * vector.x;
    result_vec.y = scale * vector.y;
    result_vec.z = scale * vector.z;
	return result_vec;
}

parser::Vec3f parser::vector_division(float scale, Vec3f vector) {
    Vec3f result_vec;
    result_vec.x = vector.x / scale;
    result_vec.y = vector.y / scale;
    result_vec.z = vector.z / scale;
    return result_vec;
}
// ----------------------------------------------------------------------

// Calculates the position with a specific ray and a t value
parser::Vec3f parser::Ray::on_point(float t) {
    return vector_sum(origin, vector_multipleS(t, direction));
}

// Calculates the t value for a specific point on a ray
float parser::Ray::get(Vec3f & p) {
    Vec3f difference = vector_subtract(p, origin);
    return (difference.x + difference.y + difference.z) / (direction.x + direction.y + direction.z);
}

// Image constructor. Allocating space for unsigned char* data 
parser::Image::Image(int width, int height) : width(width), height(height) {
    data = new unsigned char [height * width * 3];
}

// Set the rgb value of a pixel according to current row-column position
void parser::Image::pixel_rgb_set(Color &color, int col, int row, int width) {
    data[row*width*3 + 3*col] = color.rgb_val[0];
    data[row*width*3 + 3*col+1] = color.rgb_val[1];
    data[row*width*3 + 3*col+2] = color.rgb_val[2];
}

// Global row value for threads to 
int next_image_row;

// Set all pixel rgb values in a single row for all columns
void render_next_row(parser::Image *image, parser::Scene *scene, int camera_indice, const int row) {
    for (int col = 0; col < scene->cameras[camera_indice].image_width; ++col) {
        parser::Color pixel_color = pixel_render(col, row, scene, camera_indice);
        image->pixel_rgb_set(pixel_color, col, row, image->width);
    }
}

// Assign a row to a thread -> thread start to render row
void thread_function(parser::Image *image, parser::Scene *scene, int camera_indice) {
    while (true) {
        next_image_row--;
        if (next_image_row >= 0) render_next_row(image, scene, camera_indice, next_image_row);
        else break;
    }
}

// Main render function of raytracing algorithm
void parser::Scene::render_scene(void) {
    unsigned int core_count = std::thread::hardware_concurrency();
    //For each cam in input, generate another ppm file
    for (int x = 0; x < cameras.size(); ++x) {
        cameras[x].right = cross_product(cameras[x].gaze, cameras[x].up);
        parser::Image * image = new Image(cameras[x].image_width, cameras[x].image_height);
        // Set next_image_row global variable to image's highest point
        next_image_row = cameras[x].image_height;
        // Check number of cores && image height is higher than core count to separate work between threads
        if (core_count && cameras[x].image_height >= core_count) {
            std::thread* threads = new std::thread[core_count];
            for (int i = 0; i < core_count; i++) {
                threads[i] = std::thread(thread_function, image, this, x);
            }
            for (int i = 0; i < core_count; i++) {
                threads[i].join();
            }
            delete [] threads;
        } else thread_function(image, this, x);
        // Casting std::string image_name to char*
        int n = cameras[x].image_name.length();
        char file_name[n + 1];
        strcpy(file_name, cameras[x].image_name.c_str());
        // Create ppm image file
        write_ppm(file_name, image->data, cameras[x].image_width, cameras[x].image_height);
    }
}

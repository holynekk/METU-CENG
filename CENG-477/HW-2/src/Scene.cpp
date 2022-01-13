#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cmath>

#include "Scene.h"
#include "Camera.h"
#include "Color.h"
#include "Mesh.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "tinyxml2.h"
#include "Helpers.h"

using namespace tinyxml2;
using namespace std;

// Color Helper Functions
// TODO: Got "initial value of reference to non-const must be an lvalue" error
// Should I make all function arguments const??????
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Color color_sum(const Color & color_one, const Color & color_two) {
    Color c;
    c.r = color_one.r + color_two.r;
    c.g = color_one.g + color_two.g;
    c.b = color_one.b + color_two.b;
    return c;
}

Color color_subtract(const Color & color_one, const Color & color_two) {
    Color c;
    c.r = color_one.r - color_two.r;
    c.g = color_one.g - color_two.g;
    c.b = color_one.b - color_two.b;
    return c;
}

Color color_mult(const Color & color, double constant) {
    Color c;
    c.r = color.r * constant;
    c.g = color.g * constant;
    c.b = color.b * constant;
    return c;
}

Color color_divide(const Color & color, double constant) {
    Color c;
    c.r = color.r / constant;
    c.g = color.g / constant;
    c.b = color.b / constant;
    return c;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Scene helper functions
// ------------------------------------------------------------------------------------------------------------------------------------

// Modelling Transformations: Creates and returns a modeling transformation matrix which is built according to relevant transformation types (translation, rotation, scaling)
Matrix4 get_mesh_transformation_matrix(Scene * current_scene, Camera * camera, Mesh & mesh) {
    // Transformation matrix to return as a result
    Matrix4 transformation_result = getIdentityMatrix();
	// Iterate through all transformations of the given model in arguments (Mesh & model)
    for (int i = 0; i < mesh.numberOfTransformations; ++i) {
        if (mesh.transformationTypes[i] == 't') {
			// Generates translation matrix for that mesh
            Translation * t = current_scene->translations[mesh.transformationIds[i]-1];
            // Matrix from lecture slide 05 Page 10
            double translation_matrix[4][4] = {
                                                {1,0,0,t->tx},
                                                {0,1,0,t->ty},
                                                {0,0,1,t->tz},
                                                {0,0,0,1}
									        };
            transformation_result = multiplyMatrixWithMatrix(translation_matrix, transformation_result);
        } else if (mesh.transformationTypes[i] == 'r') {
			// Generates rotation matrix for that mesh (Used alternative method in lecture slides) 
            Rotation * r = current_scene->rotations[mesh.transformationIds[i]-1];
            
            // Create an orthonormal basis
            Vec3 u = Vec3(r->ux, r->uy, r->uz, -1);
            Vec3 v;
            Vec3 w;

            // To find v, set the smallest component of u (in an absolute sense) to zero
            // and swap the other two while negating one
            double smallest_component = min(abs(r->ux), min(abs(r->uy), abs(r->uz)));
            if (smallest_component == abs(r->ux)) {
                v = Vec3(0, -1 * r->uz, r->uy, -1);
            }
            else if (smallest_component == abs(r->uy)) {
                v = Vec3(-1 * r->uz, 0, r->ux, -1);
            }
            else if (smallest_component == abs(r->uz)) {
                v = Vec3(-1 * r->uy, r->ux, 0, -1);
            } else;
                
            // w = u x v
            w = crossProductVec3(u, v);
            // Normalize v and w
            v = normalizeVec3(v);
            w = normalizeVec3(w);
            // Now rotate uvw such that it aligns with xyz: call this transform M
            double M[4][4] = {
                                {u.x, u.y, u.z, 0},
                                {v.x, v.y, v.z, 0},
                                {w.x, w.y, w.z, 0},
                                {0,   0,   0,   1}
                            };
            // Finding M inverse (rotating xyz to uvw) is trivial
            double M_inverse[4][4] = {
                                        {u.x,v.x,w.x,0},
                                        {u.y,v.y,w.y,0},
                                        {u.z,v.z,w.z,0},
                                        {0,0,0,1}
                                    };
            // Rotation matrix
            double rotation_matrix[4][4] = {
                                            {1, 0,                          0,                               0},
                                            {0, cos(r->angle * M_PI / 180), -1 * sin(r->angle * M_PI / 180), 0},
                                            {0, sin(r->angle * M_PI / 180), cos(r->angle * M_PI / 180),      0},
                                            {0, 0,                          0,                               1}
                                        };
            // Final form is = [M_inverse][Rotation_x(theta)][M]
            transformation_result = multiplyMatrixWithMatrix(multiplyMatrixWithMatrix(M_inverse, multiplyMatrixWithMatrix(rotation_matrix, M)), transformation_result);
        } else if (mesh.transformationTypes[i] == 's') {
			// Generates scaling matrix for that mesh
            Scaling * s = current_scene->scalings[mesh.transformationIds[i]-1];
            // Matrix from lecture slide 05 Page 11
            double scaling_matrix[4][4] = {
                                            {s->sx,0,    0,    0},
                                            {0,    s->sy,0,    0},
                                            {0,    0,    s->sz,0},
                                            {0,    0,    0,    1}
                                        };
            transformation_result = multiplyMatrixWithMatrix(scaling_matrix, transformation_result);
        } else;
    } return transformation_result;
}

// Get camera transformation matrix: In this function, I directly calculated composite camera transformation with the formula 
Matrix4 get_camera_transformation_matrix(Camera * camera) {
    // rotation__matrix = Rotate uvw to align it with xyz
    // translation matrix = Translate camera to the world origin (0, 0, 0)
    // composite_transformation_matrix = multiplyMatrixWithMatrix(rotation__matrix, translation_matrix)
    Vec3 u = camera->u;
    Vec3 v = camera->v;
    Vec3 w = camera->w;
    Vec3 e = camera->pos;
    double composite_transformation_matrix[4][4] = {
                                                        {u.x, u.y, u.z, -(u.x * e.x + u.y * e.y + u.z * e.z)},
                                                        {v.x, v.y, v.z, -(v.x * e.x + v.y * e.y + v.z * e.z)},
                                                        {w.x, w.y, w.z, -(w.x * e.x + w.y * e.y + w.z * e.z)},
                                                        {0  , 0	 , 0  , 1}
                                                    };
    return Matrix4(composite_transformation_matrix);
}

// Get projection transformation matrix: projection matrix has been calculated with formulas in lecture notes 06 according to projection type
// For perspective
Matrix4 get_perspective_projection_matrix(Camera * camera) {
    double l = camera->left, r = camera->right, t = camera->top, b = camera->bottom, f = camera->far, n = camera->near;
    double perspective_matrix[4][4] = {
                                        {(2*n) / (r - l), 0              , (r + l) / (r - l)   , 0},
                                        {0              , (2*n) / (t - b), (t + b) / (t - b)   , 0},
                                        {0              , 0              , -((f + n) / (f - n)), -((2*f*n) / (f - n))},
                                        {0              , 0              , -1                  , 0}
                                    };
    return Matrix4(perspective_matrix);
}

// For orthagonal
Matrix4 get_orthographic_projection_matrix(Camera * camera) {
    double l = camera->left, r = camera->right, t = camera->top, b = camera->bottom, f = camera->far, n = camera->near;
    double orthographic_matrix[4][4] = {
                                        {2/(r - l), 0        , 0           , -((r + l) / (r - l))},
                                        {0        , 2/(t - b), 0           , -((t + b) / (t - b))},
                                        {0        , 0        , -(2/(f - n)), -((f + n) / (f - n))},
                                        {0        , 0        , 0           , 1}
                                    };
    return Matrix4(orthographic_matrix);
}

// For viewport transformation matrix we do not need the 4th row of the matrix, but to be able
// to use it in code we assign a dummy row ( {0, 0, 0, 1} )
Matrix4 get_viewport_transformation_matrix(Camera * camera) {
    double viewport_transformation_matrix[4][4] = {
								{camera->horRes/2.0, 				  0,   0, (camera->horRes-1)/2.0},
                               	{0				   , camera->verRes/2.0,   0, (camera->verRes-1)/2.0},
                               	{0				   , 0				   , 0.5, 0.5},
                               	{0				   , 0				   ,   0, 1}
							};
    return Matrix4(viewport_transformation_matrix);
}

// Liang-Barsky Algorithm from lecture slides 07 Page 45
bool visible(double den, double num, double & tE, double & tL) {
    double t;
    if (den > 0) {
        // Potentially entering
		t = num / den;
        if (t > tL) {
			return false;
		} else if (t > tE) {
			tE = t;
		} else;
    } else if (den < 0) {
        // Potentially leaving
		t = num / den;
        if (t < tE) {
			return false;
		} else if (t < tL) {
			tL = t;
		} else;
    } else if (num > 0) {
        // Line parallel to edge
        return false;
    } else;
    return true;
}

// Liang-Barsky Algorithm 3D extension: Lecture Slides 07 Page 46
bool line_clipping(Vec4 & first_vec, Color & first_vec_color, Vec4 & second_vec, Color & second_vec_color) {
    double dx = second_vec.x - first_vec.x, dy = second_vec.y - first_vec.y, dz = second_vec.z - first_vec.z;
    Color color_diff = color_subtract(second_vec_color, first_vec_color);

    bool is_visible = false;
    double tE = 0, tL = 1;
    double max_x = 1, max_y = 1, max_z = 1, min_x = -1, min_y = -1, min_z = -1;
    if (visible(dx, min_x-first_vec.x, tE, tL)) { // left
        if (visible(-dx, first_vec.x-max_x, tE, tL)) { // right
            if (visible(dy, min_y-first_vec.y, tE, tL)) { // bottom
                if (visible(-dy, first_vec.y-max_y, tE, tL)) { // top
                    if (visible(dz, min_z-first_vec.z, tE, tL)) { // front
                        if (visible(-dz, first_vec.z-max_z, tE, tL)) { // back
                            is_visible = true;
                            if (tL < 1) {
                                second_vec.x = first_vec.x + (dx * tL);
                                second_vec.y = first_vec.y + (dy * tL);
                                second_vec.z = first_vec.z + (dz * tL);
                                second_vec_color = color_sum(first_vec_color, color_mult(color_diff, tL));
                            } else;
                            if (tE > 0) {
                                first_vec.x = first_vec.x + (dx * tE);
                                first_vec.y = first_vec.y + (dy * tE);
                                first_vec.z = first_vec.z + (dz * tE);
                                first_vec_color = color_sum(first_vec_color, color_mult(color_diff, tE));
                            } else;
                        } else;
                    } else;
                } else;
            } else;
        } else;
    } else;
    return is_visible;
}

void apply_line_rasterization(Scene * current_scene, Vec4 & v0, Vec4 & v1, Color & c0, Color & c1) {
    int increase_with = 1;
    // Get coordinate differences of start and end pixels
    double dx = v1.x - v0.x, dy = v1.y - v0.y;
    // Check slope
    if (abs(dy) <= abs(dx)) {
        if (v1.x < v0.x) {
            swap(v0, v1);
            swap(c0, c1);
        } else;
        if (v1.y < v0.y) {
            // if (v1.y < v0.y) -> line goes in negative direction in each iteration; else increase_with remains 1
            increase_with = -1;
        } else;

        int d = v0.y - v1.y + increase_with * 0.5 * (v1.x - v0.x), y = v0.y;
        // Set the pixel color as first point's color (x) and find the color difference between starting point and ending point (color_diff)
        Color color_diff = color_divide(color_subtract(c1, c0), v1.x - v0.x), pixel_color = c0;
        // Midpoint algorithm
        for (int x = v0.x; x <= v1.x; x++) {
            // draw(x, y)
            current_scene->image[x][y] = pixel_color.round_to_closest_int();
            if (d * increase_with < 0) {
                // choose NE
                y += increase_with;
                d += (v0.y - v1.y) + (increase_with * (v1.x - v0.x));
            } else {
                // choose E
                d += (v0.y - v1.y);
            }
            // update pixel color
            pixel_color = color_sum(pixel_color, color_diff);
        }
    } else if (abs(dy) > abs(dx)) {
        if (v1.y < v0.y) {
            swap(v0, v1);
            swap(c0, c1);
        } else;
        if (v1.x < v0.x) {
            // if (v1.x < v0.x) -> line goes in negative direction in each iteration; else increase_with remains 1
            increase_with = -1;
        } else;

        int d = v1.x - v0.x + increase_with * 0.5 * (v0.y - v1.y), x = v0.x;
        // Set the pixel color as first point's color (x) and find the color difference between starting point and ending point (color_diff)
        Color color_diff = color_divide(color_subtract(c1, c0), v1.y - v0.y), pixel_color = c0;

        for (int y = v0.y; y <= v1.y; y++) {
            // draw(x, y)
            current_scene->image[x][y] = pixel_color.round_to_closest_int();
            if (d * increase_with > 0) {
                x += increase_with;
                d += (v1.x - v0.x) + (increase_with * (v0.y - v1.y));
            } else {
                d += (v1.x - v0.x);
            }
            // update pixel color
            pixel_color = color_sum(pixel_color, color_diff);
        }
    } else;
}

// Triangle rasterization: From the lecture notes 08 Page 29
void apply_triangle_rasterization( Camera * camera, Scene * scne_pointer, Vec4 & v0, Vec4 & v1, Vec4 & v2, Color * color0, Color * color1, Color * color2) {
    int hres = camera->horRes, vres = camera->verRes;
    // Finding xmin, xmax, ymin and ymax by checking camera's horizontal and vertical resolutions
    int x_min = min(v2.x, min(v0.x, v1.x));
    if (x_min < 0) {
        x_min = 0;
    } else;
    if (x_min > hres-1) {
        x_min = hres-1;
    } else;
    int y_min = min(v2.y, min(v0.y, v1.y));
    if (y_min < 0) {
        y_min = 0;
    } else;
    if (y_min > vres-1) {
        y_min = vres-1;
    } else;

    int x_max = max(max(v0.x, v1.x), v2.x);
    if (x_max < 0) {
        x_max = 0;
    } else;
    if (x_max > hres-1) {
        x_max = hres-1;
    } else;
    int y_max = max(max(v0.y, v1.y), v2.y);
    if (y_max < 0) {
        y_max = 0;
    } else;
    if (y_max > vres-1) {
        y_max = vres-1;
    } else;
    
    Color pixel_color;
    // alpha, beta, gamma values that are calculated for all pixels on image
    double a, b, g;
    // Main triangle rasterization algorithm
    for(int y = y_min; y <= y_max; ++y){
        for(int x = x_min; x <= x_max; ++x){
            // Find alpha, beta, and gamma values
            a = f12(x,y, v1.x, v1.y, v2.x, v2.y) / f12(v0.x,v0.y, v1.x,v1.y, v2.x,v2.y);
            b = f20(x,y, v2.x, v2.y, v0.x, v0.y) / f20(v1.x,v1.y, v2.x,v2.y, v0.x,v0.y);
            g = f01(x,y, v0.x, v0.y, v1.x, v1.y) / f01(v2.x,v2.y, v0.x,v0.y, v1.x,v1.y);

            if (a >= 0 && b >= 0 && g >= 0){
                pixel_color = color_sum(color_mult((*color0), a), color_sum(color_mult((*color1), b), color_mult((*color2), g)));
                // Set the pixel color
                scne_pointer->image[x][y] = pixel_color.round_to_closest_int(); 
            } else;
        }
    }
}

bool is_culled(Vec4 & v0, Vec4 & v1, Vec4 & v2) {
    // Create Vec3 types of projected Vec4 s to find edges.
    Vec3 v0_copy = Vec3(v0.x, v0.y, v0.z, v0.colorId), v1_copy = Vec3(v1.x, v1.y, v1.z, v1.colorId), v2_copy = Vec3(v2.x, v2.y, v2.z, v2.colorId);
    // Find two edges between vertex v1 - v0 and vertex vertex v2 - v0
    Vec3 edge_v1 = subtractVec3(v1_copy, v0_copy), edge_v2 = subtractVec3(v2_copy, v0_copy);
    // Use edges to find normal vector of the surface
    Vec3 normal_vector = normalizeVec3(crossProductVec3(edge_v1, edge_v2));
    // Return if the angle between normal_vector and surface to determine ??is backface culled??
    return dotProductVec3(normal_vector, v0_copy) < 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------
/*
	Transformations, clipping, culling, rasterization are done here.
	You may define helper functions.
*/
void Scene::forwardRenderingPipeline(Camera *camera) {
    // Projection transformation
    Matrix4 projection_matrix;
    if (camera->projectionType == 0) {
        // Do orthographic projection
        projection_matrix = get_orthographic_projection_matrix(camera);
    } else if (camera->projectionType == 1) {
        // Do perspective projection
        projection_matrix = get_perspective_projection_matrix(camera);
    } else;
    // Viewport transformation matrix (process after clipping)
    Matrix4 viewport_matrix = get_viewport_transformation_matrix(camera);
    // Camera transformation matrix
	Matrix4 camera_trans = get_camera_transformation_matrix(camera);

    // For every mesh in the scene, apply transformations -> clip -> cull -> rasterize
    for (auto & mesh : this->meshes) {
        Matrix4 mesh_matrix = get_mesh_transformation_matrix(this, camera, * mesh);
        // Apply other matrices on mesh_matrix -> [projection_matrix] * [camera_transformation_mattrix] * [mesh_matrix]
        Matrix4 total_mesh_matrix = multiplyMatrixWithMatrix(projection_matrix, multiplyMatrixWithMatrix(camera_trans, mesh_matrix));

        // Process all triangles
        for (Triangle & triangle : mesh->triangles) {
            // Get vertices and colors of those vertices for a single triangle
            Vec3 * vertex0 = this->vertices[triangle.getFirstVertexId()-1];
            Color * vertex0_color = this->colorsOfVertices[vertex0->colorId-1];

            Vec3 * vertex1 = this->vertices[triangle.getSecondVertexId()-1];
            Color * vertex1_color = this->colorsOfVertices[vertex1->colorId-1];

            Vec3 * vertex2 = this->vertices[triangle.getThirdVertexId()-1];
            Color * vertex2_color = this->colorsOfVertices[vertex2->colorId-1];

            // Find projected versions of those vertices
            Vec4 projected_vertex0 = multiplyMatrixWithVec4(total_mesh_matrix, Vec4(vertex0->x, vertex0->y, vertex0->z, 1, vertex0->colorId));
            Vec4 projected_vertex1 = multiplyMatrixWithVec4(total_mesh_matrix, Vec4(vertex1->x, vertex1->y, vertex1->z, 1, vertex1->colorId));
            Vec4 projected_vertex2 = multiplyMatrixWithVec4(total_mesh_matrix, Vec4(vertex2->x, vertex2->y, vertex2->z, 1, vertex2->colorId));

            // Check if backface culling is enabled and it's culled -> If yes for both -> ignore this triangle (continue with next triangle)
            if (this->cullingEnabled && is_culled(projected_vertex0, projected_vertex1, projected_vertex2)) {
                continue;
            } else;

            // if (!mesh->type) -> wireframe mode; else -> solid render
            if (!mesh->type) {
                // Grouping of lines as 0-1 | 1-2 | 2-0 respectively
                // -------------------------------------------------------------
                Vec4 line01_v0 = projected_vertex0 /= projected_vertex0.t;
                Color line01_c0 = *vertex0_color;

                Vec4 line01_v1 = projected_vertex1 /= projected_vertex1.t;
                Color line01_c1 = *vertex1_color;


                Vec4 line12_v1 = projected_vertex1 /= projected_vertex1.t;
                Color line12_c1 = *vertex1_color;

                Vec4 line12_v2 = projected_vertex2 /= projected_vertex2.t;
                Color line12_c2 = *vertex2_color;


                Vec4 line20_v2 = projected_vertex2 /= projected_vertex2.t;
                Color line20_c2 = *vertex2_color;

                Vec4 line20_v0 = projected_vertex0 /= projected_vertex0.t;
                Color line20_c0 = *vertex0_color;
                // -------------------------------------------------------------

                // Do line clipping for those lines
                bool L01_is_visible = line_clipping(line01_v0, line01_c0, line01_v1, line01_c1);
                bool L12_is_visible = line_clipping(line12_v1, line12_c1, line12_v2, line12_c2);
                bool L20_is_visible = line_clipping(line20_v2, line20_c2, line20_v0, line20_c0);

                // Viewport transformation: already got viewport_matrix at the beginning. Juts multiply it.

                line01_v0 = multiplyMatrixWithVec4(viewport_matrix, line01_v0);
                line01_v1 = multiplyMatrixWithVec4(viewport_matrix, line01_v1);

                line12_v1 = multiplyMatrixWithVec4(viewport_matrix, line12_v1);
                line12_v2 = multiplyMatrixWithVec4(viewport_matrix, line12_v2);

                line20_v2 = multiplyMatrixWithVec4(viewport_matrix, line20_v2);
                line20_v0 = multiplyMatrixWithVec4(viewport_matrix, line20_v0);

                // Rasterize the triangle
                if(L01_is_visible) {
                    apply_line_rasterization(this, line01_v0, line01_v1, line01_c0, line01_c1);
                } else;
                    
                if(L12_is_visible) {
                    apply_line_rasterization(this, line12_v1, line12_v2, line12_c1, line12_c2);
                } else;
                    
                if(L20_is_visible) {
                    apply_line_rasterization(this, line20_v2, line20_v0, line20_c2, line20_c0);
                } else;
                    
            } else {
                // Solid rendering
                // First apply perspective division and then viewport transformation for all vertices
                projected_vertex0 /= projected_vertex0.t;
                Vec4 viewportV0 = multiplyMatrixWithVec4(viewport_matrix, projected_vertex0);
                projected_vertex1 /= projected_vertex1.t;
                Vec4 viewportV1 = multiplyMatrixWithVec4(viewport_matrix, projected_vertex1);
                projected_vertex2 /= projected_vertex2.t;
                Vec4 viewportV2 = multiplyMatrixWithVec4(viewport_matrix, projected_vertex2);
                // Rasterize and fill in the triangle
                apply_triangle_rasterization(camera, this, viewportV0, viewportV1, viewportV2, vertex0_color, vertex1_color, vertex2_color);
            }
        }
    }
}

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL) {
		str = pElement->GetText();
		
		if (strcmp(str, "enabled") == 0) {
			cullingEnabled = true;
		}
		else {
			cullingEnabled = false;
		}
	}

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera *cam = new Camera();

		pCamera->QueryIntAttribute("id", &cam->cameraId);

		// read projection type
		str = pCamera->Attribute("type");

		if (strcmp(str, "orthographic") == 0) {
			cam->projectionType = 0;
		}
		else {
			cam->projectionType = 1;
		}

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

		cam->gaze = normalizeVec3(cam->gaze);
		cam->u = crossProductVec3(cam->gaze, cam->v);
		cam->u = normalizeVec3(cam->u);

		cam->w = inverseVec3(cam->gaze);
		cam->v = crossProductVec3(cam->u, cam->gaze);
		cam->v = normalizeVec3(cam->v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam->left, &cam->right, &cam->bottom, &cam->top,
			   &cam->near, &cam->far, &cam->horRes, &cam->verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam->outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		vertices.push_back(vertex);
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		Translation *translation = new Translation();

		pTranslation->QueryIntAttribute("id", &translation->translationId);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		translations.push_back(translation);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		Scaling *scaling = new Scaling();

		pScaling->QueryIntAttribute("id", &scaling->scalingId);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		scalings.push_back(scaling);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		Rotation *rotation = new Rotation();

		pRotation->QueryIntAttribute("id", &rotation->rotationId);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		rotations.push_back(rotation);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read meshes
	pElement = pRoot->FirstChildElement("Meshes");

	XMLElement *pMesh = pElement->FirstChildElement("Mesh");
	XMLElement *meshElement;
	while (pMesh != NULL)
	{
		Mesh *mesh = new Mesh();

		pMesh->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = pMesh->Attribute("type");

		if (strcmp(str, "wireframe") == 0) {
			mesh->type = 0;
		}
		else {
			mesh->type = 1;
		}

		// read mesh transformations
		XMLElement *pTransformations = pMesh->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		while (pTransformation != NULL)
		{
			char transformationType;
			int transformationId;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *clone_str;
		int v1, v2, v3;
		XMLElement *pFaces = pMesh->FirstChildElement("Faces");
        str = pFaces->GetText();
		clone_str = strdup(str);

		row = strtok(clone_str, "\n");
		while (row != NULL)
		{
			sscanf(row, "%d %d %d", &v1, &v2, &v3);
			mesh->triangles.push_back(Triangle(v1, v2, v3));
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		meshes.push_back(mesh);

		pMesh = pMesh->NextSiblingElement("Mesh");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}
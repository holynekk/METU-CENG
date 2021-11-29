#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>

namespace parser
{
    struct Vec3f
    {
        float x, y, z;
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Color {
        unsigned char rgb_val[3];
    };

    struct Return_Intersection {
        float t;
        int materialId;
        Vec3f normal;
    };

    Vec3f vector_sum(Vec3f vec1, Vec3f vec2);
    Vec3f vector_subtract(Vec3f vec1, Vec3f vec2);
    Vec3f vector_multipleS(float num, Vec3f vec);
    Vec3f vector_division(float num, Vec3f vec);

    struct Ray {
        Vec3f origin;
	    Vec3f direction;

        // Constructors
        Ray(){};
        Ray(const Vec3f &origin, const Vec3f& direction) : origin(origin), direction(direction){};
        // Functions
        float get(Vec3f &p);
        Vec3f on_point(float t);
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec3f right; // assigned at renderScene function
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;

        Ray generate_prim_ray(int col, int row);
    };

    struct Image {
        unsigned char* data;
        int width;
        int height;

        Image(int width, int height);
        void pixel_rgb_set(Color &color, int col, int row, int width_val);
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;

        Vec3f calc_light_contribution(Vec3f &p);
    };

    struct Material
    {
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent = 0;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;

        // This intersect function reached from Mesh object
        Return_Intersection intersect(Ray &ray, int material_id);
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;

        Return_Intersection intersect(Ray & ray);
    };

    struct Triangle
    {
        int material_id;
        Face indices;

        Return_Intersection intersect(Ray & ray);
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;

        Return_Intersection intersect(Ray & ray);
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string &filepath);
        void render_scene(void);
    };
}

#endif

#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>

namespace parser
{
    struct Vec3f
    {
        union 
        {
            float x;
            float r;
        };
        union
        {
            float y;
            float g;
        };
        union
        {
            float z;
            float b;
        };
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    typedef union Color {
        struct {
            unsigned char red;
            unsigned char grn;
            unsigned char blu;
        };
        unsigned char channel[3];
    } Color;

    struct IntersectionData {
        float t;
        Vec3f normal;
        // Vec3f intersection;
        int materialId;
    };

    Vec3f vector_sum(Vec3f first, Vec3f second, int type);
    Vec3f vector_subtract(Vec3f first, Vec3f second, int type);
    Vec3f vector_multipleS(float scale, Vec3f vector, int type);
    Vec3f vector_division(float scale, const Vec3f & vector, int type = 0);

    typedef struct Ray {

        Vec3f origin;
	    Vec3f direction;
        Ray(){};
        Ray(const Vec3f& origin, const Vec3f& direction) : origin(origin), direction(direction){};
        Vec3f getPoint(float t);
        float gett(const Vec3f & p);

    } Ray;

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

        Ray getPrimaryRay(int col, int row);
    };

    struct Image {
        Color** data;
        int width;
        int height;

        Image(int width, int height);
        void setPixelValue(int col, int row, const Color& color);
        void saveImage(std::string imageName);	
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;

        Vec3f computeLightContribution(const Vec3f& p);
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
        IntersectionData intersect(const Ray & ray, int material_id);
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;

        IntersectionData intersect(const Ray & ray);
    };

    struct Triangle
    {
        int material_id;
        Face indices;

        IntersectionData intersect(const Ray & ray);
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;

        IntersectionData intersect(const Ray & ray);
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
        void renderScene(void);
    };
}

#endif

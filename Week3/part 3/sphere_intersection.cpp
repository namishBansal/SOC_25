#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <sycl/sycl.hpp>
#include <vector>
#include <fstream>
#include <iostream>

SYCL_EXTERNAL bool hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    auto b = -2.0 * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    return (discriminant >= 0);
}

SYCL_EXTERNAL color ray_color(const ray& r) {
    if (hit_sphere(point3(0,0,-1), 0.5, r))
        return color(1, 0, 0);

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center
                             - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::vector<color> framebuffer(image_width * image_height);

    // Setup ray params
    auto origin = camera_center;
    auto horizontal = viewport_u;
    auto vertical = viewport_v;
    auto lower_left_corner = pixel00_loc;


    // Render

     sycl::queue q;
    {
        sycl::buffer<color> fb_buf(framebuffer.data(), framebuffer.size());

        q.submit([&](sycl::handler& h) {
            auto acc = fb_buf.get_access<sycl::access::mode::write>(h);

            h.parallel_for(sycl::range<2>(image_height, image_width), [=](sycl::id<2> idx) {
                int j = idx[0];
                int i = idx[1];
                int pixel_index = j * image_width + i;

                auto u = double(i) / (image_width - 1);
                auto v = double(image_height - 1 - j) / (image_height - 1);  // Flip Y

                ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
                acc[pixel_index] = ray_color(r);
            });
        });
    }

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (auto& c : framebuffer)
        write_color(std::cout, c);
}
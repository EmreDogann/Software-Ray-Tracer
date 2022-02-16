# Software Ray Tracing
My implementation of the [Ray Tracing In One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) series.

# Main features:
- Materials (Lambertain, Metallic, Dielectric)
- Camera Positioning
- MSAA
- Ray interactions:
  - Reflection
  - Refraction/Transmission
- Global-Illumination
- Multi-Threading

# Multi-Threading
For Multi-Threading, I have designed the implementation as follows:
- Spawn **X** number of threads (by default will spawn the max number of threads available).
- Using atomic variables, each thread will pick a row to compute.
  - Once a row has been finished, the thread will move onto the next free row.
- Once all rows have been calculated, they are reordered in an array and then written to a .ppm file.

The image below has a resolution of **2560x1440**. Rendered with 100 samples per pixel with a ray depth of 50.

On a Ryzen 7 3700x, utlizing all 16 threads at 4.0GHz all-core, the entire image was rendered in 5 minutes 46 seconds. This is almost a 10x improvement on the inital single-threaded version.

![Image](./images/Image.png)

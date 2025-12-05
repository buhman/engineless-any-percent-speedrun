unsigned int compile_shader(const void * vp,
                            const size_t vp_length,
                            const void * fp,
                            const size_t fp_length);

int make_buffer(unsigned int target,
                const void * data,
                size_t size);

int make_texture(const void * data,
                 int internalformat,
                 int width,
                 int height,
                 int format);

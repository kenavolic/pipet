# Pipet

[![Build Status](https://travis-ci.org/kenavolic/pipet.svg?branch=master)](https://travis-ci.org/kenavolic/pipet)

Pipet is a lightweight c++17 headers-only library than can be used to build
simple processing pipelines at compile time.

# Features

  * Compile-time pipeline building from a set of user-defined filters
  * Compile-time or runtime pipeline execution
  * Creation of reversible pipelines (if all filters in the pipeline are reversible)
  * Creation of branches (handling of multiple input filters)

# Supported Platforms

Tests have been performed on the following platforms:

  * g++-7 on linux (ci)
  * clang++-7 (ci)

# Install

  * Clone project
~~~
    > git clone https://github.com/kenavolic/pipet.git
~~~

  * Generate build system
~~~
    > mkdir pipet_build
    > cd pipet_build
    > cmake -DCMAKE_INSTALL_PREFIX=$path_to_pipet_install_dir -DPIPET_BUILD_EXAMPLES=[ON|OFF] -DPIPET_BUILD_TESTS=[ON|OFF] ../pipet
~~~

  * Compilation
~~~
    > make
    > make install
~~~
    + /!\ On windows, open generated solution and build solution and install target


# Usage

## Examples

The projet include the following examples:

* AES ciphering at compile time
* String obfuscation at compile time
* Mask generation at compile time

## Import pipet to your project

  * In your CMakeLists.txt, import pipet
~~~
    find_package(pipet REQUIRED)
    ...
    target_link_libraries(my_proj pipet::pipet)
~~~

  * Configure your project to find pipet package
~~~
    > cmake -DCMAKE_PREFIX_PATH=$path_to_pipet_install_dir/cmake $path_to_your_proj_source_dir
~~~

## Implement a pipeline

The example directory provide two examples of pipet usage. Basically, all you need to do is:

  * Create your filters with a static process and optionnaly static reverse function
~~~       
    struct filter
    {
        static constexpr out_type process(in_type my_param)
        {
            // do processing
            return my_result;
        }
        
        static constexpr in_type reverse(out_type my_param)
        {
            // do processing
            return my_result;
        }
    };
~~~

  * Build your pipeline
~~~
    using my_processing_pipe = pipet::pipe<filter1, filter2, filter3>;
~~~

  * Run it
~~~
    constexpr auto res = my_processing_pipe::process(); // if filter 1 is a data generator (no input type)
    constexpr auto res = my_processing_pipe::process(var); // if filter 1 is processing filter
~~~

  * Create branches
~~~
  struct f1_proc_ct {
    static constexpr auto process(int a) { return a; }
  };

  // compute a*a
  struct f_square_ct {
    static constexpr auto process(int a) { return a * a; }
  };

  // compute a*a*a
  struct f_cube_ct {
    static constexpr auto process(int a) { return a * a * a; }
  };

  // add 3 different inputs
  struct f_add3_ct {
    static constexpr auto process(int a, int b, int c) { return a + b + c; }
  };

  using branch1_t = pipet::pipe<f1_proc_ct, f_square_ct>;
  using branch2_t = pipet::pipe<f_cube_ct, f1_proc_ct>;

  // y = x*x + x*x + x*x*x
  using pipe_with_branches_t =
      pipet::pipe<f1_proc_ct, pipet::branches<branch1_t, branch1_t, branch2_t>,
                  f_add3_ct>;
  
  static_assert(pipe_with_branches_t::process(2) == 16,
                "[-][pipet_test] pipe processing failed");
~~~

  * Create branches with a direct connection
~~~
  ... (see previous sample)

  // y = x + x*x + x*x*x
  using pipe_with_direct_branches_t = pipet::pipe<
      f1_proc_ct,
      pipet::branches<pipet::placeholders::self, branch1_t, branch2_t>,
      f_add3_ct>;
  static_assert(pipe_with_direct_branches_t::process(2) == 14,
                "[-][pipet_test] pipe processing failed");

~~~
 


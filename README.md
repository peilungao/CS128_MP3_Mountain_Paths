---
url: https://cs128.org/2026a/mp3-mountain-paths-1702
---
# Background

In this MP, you are writing and testing code that will read in elevation data, process it, and then print out a grayscale topological image map. This map will have multiple potential paths drawn in red, with the best path in green. Descriptions of how to implement each of these parts are described below. You got this!

## Elevation Dataset

In this MP, you will read elevation data in the form of a white-space delimited list of integer values. This is consistent with how the NOAA(National Oceanic and Atmospheric Administration) provides elevation data. Each integer represents the average elevation in a cell's meters in the topological data grid. The list of integers maps directly onto a rectangular grid to represent the elevation of an area of land. For instance, the following topological data of a 2 by 5 area of land:

|  | **col 0** | **col 1** | **col 2** | **col 3** | **col 4** |
| --- | --- | --- | --- | --- | --- |
| **row 0** | 10 | 20 | 30 | 40 | 35 |
| **row 1** | 25 | 20 | 15 | 18 | 20 |

 Could be given to you in the format of:

| 10 | 20 | 30 | 40 | 35 | 25 | 20 | 15 | 18 | 20 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |

The first 5 integers encode the numbers for the first row (i.e., row 0), and the next five integers for the second row (i.e., row 1):

The above example is a simplified example used for explanation. In the actual data provided by NOAA, each integer represents the average elevation of a 700-meter by 700-meter square of land. We suggest you formulate some fictional, simple samples by hand for testing purposes.

A goal in this assignment is to translate the file representation of a data sample into an `ElevationDataset` object. The data in the file will be given as a continuous stream of numbers -- there may or may not be any line breaks in the file. Notice that the constructor of this object takes the path to the plain-text file to be read, as well as the composition of the dataset (i.e., width and height) that it describes (the width and height are not provided in the data file).
 During the construction of an `ElevationDataset` object, you must initialize the `width_` and `height_` members with the values stored in their corresponding parameters. You will write the elevation data to `ElevationDataset` object's `data_` member in row-major form [row, col]. During this process, you should be recording the minimum and maximum elevation datum values observed in the dataset in `min_ele_` and `max_ele_`, respectively.
While translating the file to matrix form, you should validate the input from the user. The number of elevation points in the file should be exactly `width * height`. You must throw an exception if there are too many data points or too few in the file. Furthermore, you must throw an exception if an error results from a formatted-read failure. An `std::runtime_error` with an adequate description of the exceptional case is sufficient.

**Instructor's recommendation**: I would encourage you to initialize the `data_` vector as a pre-allocated vector of height "rows" with width "columns" to save run-time(see Two-dimensional vectors lesson). You can then use a nested for-statement to attempt to read `width_ * height_` data values into the proper index in the two-dimensional vector. For example,

```cpp
initialize data_ as pre-allocated two-dimensional vector of height_ rows, width_ columns
for i in 0 until height_ {
  for j in 0 until width_ {
    attempt to read from the file
    if read fails
      throw exception // too little data in a file or format read error
    else
      assign data_.at(i).at(j) the value read from file
  }
}
check the file for too much data
```

## Grayscale topological image map formulation

The next step is constructing a grayscale topological image from the elevation data. We do this by scaling our elevation data to construct an image where high-brightness areas represent high elevations and dark regions have low elevations. In translating to this representation, we will encode an elevation datum as a grayscale color using the RGB color model. This model blends red, green, and blue components to construct colors. When this model presents a grayscale color, the red, green, and blue components take on the same integer value between 0-255 (i.e., the valid color component range, inclusive).

Therefore, to calculate the shade of gray for an elevation datum, we must scale that elevation point between 0 and 255. This scaling requires us to leverage the maximum value elevation datum observed in the dataset as well as the lowest. With this information, the shade of gray of any elevation point is calculated as

```cpp
shade_of_gray = std::round((elevation_point - min_ele) / (max_ele - min_ele) * 255)
```

You should use [std::round](https://en.cppreference.com/w/cpp/numeric/math/round) defined in header `<cmath>` to perform the rounding. **Pay special attention to the types you are dividing otherwise information will be lost in the computation.**

If the denominator in this equation evaluates to `0` (i.e., `max_ele == min_ele`), the `shade_of_gray` should take on `0`. The grayscale RGB value for the elevation value's grid will take on `shade_of_gray` for the `R`, `G`, and `B` components of the RGB color model. We create a `Color` object to represent a color in our program. For the grayscale image, we construct a `GrayscaleImage` object that encodes the topological map by housing a `std::vector<std::vector<Color>>`. For example, given the following `data_` from an ElevationDataset object,

|  | **0** | **1** | **2** | **3** | **4** |
| --- | --- | --- | --- | --- | --- |
| **0** | 10 | 20 | 30 | 40 | 35 |
| **1** | 25 | 20 | 15 | 18 | 20 |

A `GrayscaleImage` object’s `image_` would take on the following composition -- after scaling the elevation data between 0 and 255:

|  | **0** | **1** | **2** | **3** | **4** |
| --- | --- | --- | --- | --- | --- |
| **0** | Color(0,0,0) | Color(85,85,85) | Color(170,170,170) | Color(255,255,255) | Color(213,213,213) |
| **1** | Color(128,128,128) | Color(85,85,85) | Color(43,43,43) | Color(68,68,68) | Color(85,85,85) |

which represents the following colors for each grid position:

|  | **0** | **1** | **2** | **3** | **4** |
| --- | --- | --- | --- | --- | --- |
| **0** |  |  |  |  |  |
| **1** |  |  |  |  |  |

## Grayscale topological image map overlaid with colored paths

Imagine you are hiking in the Colorado Rockies and would like to know the path of least elevation change (i.e., the path of least resistance) from one side of the mountain to the other. Given the topological data for the region that you are in, one can calculate the path of least resistance from one side of the topological image map to the other. In this assignment, a `PathImage` object will be initialized with the topological image map, elevation data, and the width and height of the dataset. Starting from each data cell in the first (0th) column, a greedy least-elevation-change walk will be calculated from the first column to the last. The path found from each row will be encoded as a `Path` object and stored in the `PathImage` object's `paths_` collection. This will result in a `height` number of paths stored in the `PathImage` object. During construction, the `PathImage` object's `path_image_` is initialized with a copy of the argument passed to the parameter `image`. Before construction of the `PathImage` object has finished, each path will be painted across `path_image_` in red [RGB(252,25,63)], with the path of least overall resistance colored in green [RGB(31,253,13)]. **If there are paths of equal least overall resistance**, color green, the path begins from the lowest "row" number.

For instance, given a `GrayscaleImage` object’s `image_` encoding the following topological image map:
 ![mountain_paths_1](https://cdn1.cs128.org/sp21/mountain_paths_1.jpg)
The `PathImage` object’s `path_image_` derived from the `GrayscaleImage` object’s `image_` shown above would encode:
 ![mountain_paths_2](https://cdn1.cs128.org/sp21/mountain_paths_2.png)
Notice that exactly one path starts from each row in the zeroth column spanning the topological image map. While paths can merge, each is considered independent from another and will have its own observed elevation change stored in its representation (`Path` object). To emphasize, the path with the least overall total change in elevation is colored in green.

### Calculating paths: a greedy walk across the image (read carefully!)

We will calculate `height_` number of paths through our elevation dataset, ultimately drawing each path upon the topological image map to create a `path_image_`. To calculate a path, we will "walk" across the elevation data, starting at the cell on the left-most edge (column 0) and proceeding forward by taking a "step" into one of the 3 adjacent cells (if valid -- `current_row`, `current_row + 1`, `current_row -1`) in the next column over (column 1). As we traverse the image, each step will always be in the next column; however, the row we end up in may differ from the one we started our "step" from.

Let's briefly consider directionality. The top-left corner of our image is at (0, 0) in the vector and bottom-right at (height-1, width-1). North is towards row 0, south towards row height-1. West is towards column 0, east is towards column width-1. Therefore, a southeast move is towards the bottom-right corner of the image.

Let's talk specifics about the greedy walk through the image. We will always choose the cell with the least elevation change. In the case of a tie with the straight-forward position (east direction), choose to go straight forward. If there is a tie between the two non-forward locations, always choose the southeast movement. The following diagrams illustrate a few of these rules:
 ![mountain_paths_3](https://cdn1.cs128.org/23c/mountain_paths_3.png)

| **Case 1**: smallest change is 5, go fwd-down | **Case 2**: smallest change is (3, go fwd | **Case 3**: smallest change is a tie (3), fwd is an option, so go fwd | **Case 4**: smallest change is a tie (4), go fwd-down |
| --- | --- | --- | --- |

How do we define the least elevation change? During our walk, the total elevation change "experienced" by a person walking this path needs to be maintained and is considered the absolute value of the change in each step summed across the total path. This value will be stored in the `Path` object's `ele_change_` data member and will be compared against other `Path` object values when determining the best path amongst all the paths. How do we encode the path? Using a `Path` object, of course. Given the following image,
 ![mountain_paths_4](https://cdn1.cs128.org/sp21/mountain_paths_4.png)
we would construct a `Path` object encoding this traversal as:

| 2 | 1 | 2 | 3 | 4 | 4 |
| --- | --- | --- | --- | --- | --- |

Explanation: `path_.at(0)` says our path starts at `row 2` of `col 0` in the image; `path_.at(1)` tells us our row location for `col 1 => row 1`; `path_.at(2) => row 2`; `path_.at(3) => row 3`; `path_.at(4) => row 4`; `path_.at(5) => row 4`.

Recall that there will be a `height_` number of `Path` objects contained in the `PathImage` object's `paths_` collection (one for each row of the dataset). Again, we will use these objects to "paint" paths across the `PathImage` object's `path_image_`. Don't skip creating the path objects: we test that you have satisfactorily computed them.

Notice that our approach in formulating a path constitutes a greedy approach. Each step taken during our walk is simply chosen as the one that seems best. This can take us to regions of topography that are rather expensive to traverse, with large elevation changes. This could have been avoided by considering every path from a given row through our dataset. Such an approach becomes expensive rather quickly: if we must step into `width_` columns to traverse the region, the number of possible paths is close to 3`width_`. Searching for a solution this way does not scale very well, especially considering that we must calculate `height_` number of paths. The greedy solution is a lot simpler, albeit with absolutely no guarantee that its solution is the least expensive one (here, in terms of least elevation change).

### Writing PPM images

`PathImage` and `GrayscaleImage` objects both have behaviors that write out PPM images. PPM stands for Portable Pixel Map and is an image encoding scheme used to represent images using the RGB color model. Its simplicity makes it suitable for this assignment, though it is inefficient (does not apply data compression, etc.) and is not widely used in practice.

A plain PPM file has the following format:

- A "magic number" for identifying the file type, *followed by a new line*.
  - We will use "`P3`" in this assignment to identify a plain PPM file
- The width followed by the height of the image in pixels. Both are integers separated by a space.
- On its own line, the maximum color value, which must be between 0 and 65536, exclusive.
  - For our assignment, this value will be 255.
- Each line represents each pixel's R, G, and B values.
- Include an empty line at the end of the file.

For example, given a `GrayscaleImage` object whose `image_` takes on the following composition:

|  | **0** | **1** | **2** | **3** | **4** |
| --- | --- | --- | --- | --- | --- |
| **0** | Color(0,0,0) | Color(85,85,85) | Color(170,170,170) | Color(255,255,255) | Color(213,213,213) |
| **1** | Color(128,128,128) | Color(85,85,85) | Color(43,43,43) | Color(68,68,68) | Color(85,85,85) |

We would write out the PPM file for this object's `image_` as:

```dat
P3
5 2
255
0 0 0 85 85 85 170 170 170 255 255 255 213 213 213
128 128 128 85 85 85 43 43 43 68 68 68 85 85 85
```

### Viewing PPM images

- Mac users -- open the ppm file you've created using the "Preview" application.
- Windows/Linux users -- as necessary, use an online free tool to convert your PPM file into a format that your system can natively read (jpg, png, etc.)

# Assignment

## Starter code

Acquire the starter code by clicking the "GitHub Classroom" button at this page's top.

## Deliverables

### Implementation

# class ElevationDataset

| Function Signature | Behavior Description |
| --- | --- |
| `ElevationDataset(const std::string& filename, size_t width, size_t height)` | Initializes the primitive data members with their respective values as read from the file; populates the two-dimensional `std::vector<std::vector<int>>` with elevation data from file. Sets `width_` and `height_` appropriately. Records `max_ele_` and `min_ele_` observed. |
| `size_t Width() const` | Returns the “width” of the dataset |
| `size_t Height() const` | Returns the “height” of the dataset |
| `int MaxEle() const` | Returns the maximum elevation value observed in the dataset |
| `int MinEle() const` | Returns the minimum elevation value observed in the dataset |
| `int DatumAt(size_t row, size_t col) const` | Returns the value recorded for (`row`, `col`) |
| `const std::vector<std::vector<int> >& GetData() const` | Returns reference to const to `data_`. |
| Data Member | Description |
| `std::vector<std::vector<int> > data_` | Houses the elevation data read from the file. |
| `size_t width_` | "Width" of the dataset as read from the file. |
| `size_t height_` | "Height" of the dataset as read from the file. |
| `int max_ele_` | Maximum elevation datum read from the file. |
| `int min_ele_` | Minimum elevation datum read from the file. |

# class GrayscaleImage

| Function Signature | Behavior Description |
| --- | --- |
| `GrayscaleImage(const ElevationDataset& dataset)` | Initializes the primitive data members with their respective values as read from the dataset; populates the two-dimensional `std::vector<std::vector<Color>>` with `Color` representation of each elevation datum. Sets `width_` and `height_` appropriately. |
| `GrayscaleImage(const std::string& filename, size_t width, size_t height)` | `filename` contains elevation data. It is reasonable to create an `ElevationDataset` object in this function and use it to implement the desired behavior of this constructor. This constructor initializes the primitive data members with their respective values as read from the file; populates the two-dimensional `std::vector<std::vector<Color>>` with translated `Color` representation from each read elevation datum. Sets `width_` and `height_` appropriately. |
| `size_t Width() const` | Returns the width of the image |
| `size_t Height() const` | Returns the height of the image |
| `unsigned int MaxColorValue() const` | Returns the value stored in `kMaxColorValue` |
| `const Color& ColorAt(int row, int col) const` | Returns the `Color` at `row` `col` by reference to const |
| `const std::vector<std::vector<Color> >& GetImage() const` | Returns reference to const to `image_`. |
| `void ToPpm( const std::string& name ) const` | Writes out `image_` in plain PPM format; filename is `name`. |
| Data Member | Description |
| `std::vector<std::vector<Color> > image_` | Image representation of converted elevation data stored as `Color` values |
| `size_t height_` | Height of the image |
| `size_t width_` | Width of the image |
| `static const int kMaxColorValue = 255;` | Maximum possible grayscale value for the image; shared across all instances of this type. |

# class PathImage

| Function Signature | Behavior Description |
| --- | --- |
| `PathImage(const GrayscaleImage &image, const ElevationDataset &dataset)` | Initializes the primitive data members with their respective values read from `image`; populates the two-dimensional `std::vector<std::vector<Color>>` with values from the image's `image_`. Calculates and stores all paths through the image; you may wish to maintain the row of the best path in a variable `best_path_row_`. |
| `size_t Width() const` | Returns the width of the image |
| `size_t Height() const` | Returns the height of the image |
| `unsigned int MaxColorValue() const` | Returns the value stored in `kMaxColorValue` |
| `const std::vector<Path>& Paths() const` | Returns reference to const to `paths_`. |
| `const std::vector<std::vector<Color> >& GetPathImage() const` | Returns reference to const to `path_image_`. |
| `void ToPpm(const std::string& name) const` | Writes out `path_image_` in plain PPM format; filename is `name`. |
| Data Member | Description |
| `std::vector<Path> paths_` | Vector storing the paths traversals calculated on `image_` from each row; `paths_.at(0)` yields the `Path` object encoding the traversal across the image starting from row 0; `paths_.at(1)` yields that from row 1; etc. |
| `std::vector<std::vector<Color> > path_image_` | Original image overlaid with paths: the best path is colored green [RGB(31,253,13)]; every other path is red [RGB(252,25,63)]. |
| `size_t height_` | Height of the image |
| `size_t width_` | Width of the image |
| `static const int kMaxColorValue = 255` | Maximum possible grayscale value for the image; shared across all instances of this type. |

# class Path

| Function Signature | Behavior Description |  |  |  |  |
| --- | --- | --- | --- | --- | --- |
| `Path(size_t length, size_t starting_row)` | Initializes the primitive data members with their respective values from arguments; initializes `path_` `std::vector<size_t>` of `length` elements. |  |  |  |  |
| `size_t Length() const` | Returns the `length_`. |  |  |  |  |
| `size_t StartingRow() const` | Returns the `starting_row_`. |  |  |  |  |
| `unsigned int EleChange() const` | Returns the `ele_change_`. |  |  |  |  |
| `void IncEleChange(unsigned int value)` | Increments `ele_change_` by value (must be positive). |  |  |  |  |
| `const std::vector<size_t>& GetPath() const` | Returns reference to const to `path_` |  |  |  |  |
| `void SetLoc( size_t col, size_t row )` | Assigns the value of `row` (`path_.at(col)` will evaluate to `row`) |  |  |  |  |
| Data Member | Description |  |  |  |  |
| `std::vector<size_t> path_` | 1 row by `length_` columns. This `std::vector<int>` encodes the path starting at row `starting_row` across the image (from "left" to "right"). Each "index" of `path_` represents a column and stores the row index of the path as it traverses that column. |  |  |  |  |
| For example, given the following image, we could construct a `Path` object describing our traversal across it (from left to right, column 0 to column width-1) from row 2 (traversals from any row always start at column 0): ![mountain_paths_4](https://cdn1.cs128.org/sp21/mountain_paths_4.png) as:<br/>\| 2 \| 1 \| 2 \| 3 \| 4 \| 4 \|<br/>\| --- \| --- \| --- \| --- \| --- \| --- \|<br/>Explanation: `path_.at(0)` says our path starts at `row 2` of `col 0` in the image; `path_.at(1)` tells us our row location for `column 1` (returns `1` representing `row 1`); `path_.at(2) => row 2`; `path_.at(3) => row 3`, `path_.at(4) => row 4`, `path_.at(5) => row 4`. |  |  |  |  |  |
| 2 | 1 | 2 | 3 | 4 | 4 |
| `size_t length_` | Number of columns in `path_`. |  |  |  |  |
| `size_t starting_row_` | Row that this path begins from. |  |  |  |  |
| `unsigned int ele_change_` | Total elevation change "experienced" by a person walking the path. |  |  |  |  |

# class Color

| Function Signature | Behavior Description |
| --- | --- |
| `Color()` | Assigns each data member its zero value. |
| `Color(int r, int g, int b)` | Initializes a new `Color` object, ensuring the values assigned to `red_`, `green_`, and `blue_` are each within the range 0 to 255 inclusive. Throw an exception if any of the arguments are outside that range. |
| `int Red() const` | Returns value stored in `red_`. |
| `int Blue() const` | Returns value stored in `blue_`. |
| `int Green() const` | Returns value stored in `green_`. |
| Data Member | Description |
| `static constexpr int kColorValueMin = 0` | Minimum value that a color component can take on. |
| `static constexpr int kColorValueMax = 255` | Maximum value that a color component can take on. |
| `int red_` | Red component of color; invariant: value in range 0-255 inclusive. |
| `int green_` | Green component of color; invariant: value in range 0-255 inclusive. |
| `int blue_` | Blue component of color; invariant: value in range 0-255 inclusive. |

## Test cases

We require you to take a test-driven approach when implementing your solution. Accordingly, you must write sufficient unit tests for each behavior afforded by the public interface.

You are required to create small test files for each of the cases under the "Calculating paths" section of this document. If you need help with this, please attend office hours on MONDAY. We will only provide support during office hours once you have made an honest effort to compose these test files.

To get you started, we have provided some examples of test files inside the `example-data`. The `all-tie-row1-2w-3h.dat` inside `input_data` provides an example for one of the test files under the "Calculating paths" section of this document. It would help if you created additional test cases similar to this for the other test cases. RememberDon't to write test cases where you begin calculating a path from start in row 0 and row size-1. Making moves from these rows requires careful consideration since only some potential moves are valid.

Here's how you should use these test files. First, create your input test files. Calculate the expected output files for each stage by hand when going from elevation data to path image. Implement `ElevationDataset` and exercise with your test input files. Does the input data get into the correct locations in the multi-dimensional vector? Are the data members maintaining the width and height set properly? Once you are confident this class has been implemented correctly, you move on to `GrayscaleImage` and begin a similar testing process there. Finally, `PathImage` should be implemented and tested accordingly.

You should also satisfactorily exercise the supporting classes `Path` and `Color`.

### Visual debugging

Note: if you're running your program with our visual debugger, please provide the absolute path to the input file. For example, if you're developing in `~/some/where/mp-mountain-paths-michaelrnowak/` and your test case is in `./example-data/ex_input_data/input-test-1.dat` relative to that directory, you will need to find the absolute path to `input-test-1.dat`. The absolute path to the tests directory can be found by `cd` into `./example-data/ex_input_data/` and executing `pwd`.

### Constraints

- You cannot create additional data members (e.g., size_, etc.): we break encapsulation to set data members to specific values and then test your implemented behaviors. We only update the data members specified in our prompt during that process.
- All functions of the public interface will be used in testing, so it is CRUCIAL that their signatures and return types are not altered (and that you implement them).
- Your program must compile without warnings/errors when compiled with: `clang++` using the `-std=c++20` and the following flags `-Wall -Wextra -Werror -pedantic`
- Only the following header files are allowed to be `#include`d in your solution files: "color.hpp" "elevation_dataset.hpp" "grayscale_image.hpp" "path_image.hpp" "path.hpp" "stdexcept" "cassert" "iostream" "fstream" "limits" "cstdlib" "string" "vector" "cmath" "utility"

### Submitting your work

- You will submit the following header and source files to PrairieLearn for grading: `color.cc`, `color.hpp`, `elevation_dataset.cc`, `elevation_dataset.hpp`, `grayscale_image.cc`, `grayscale_image.hpp`, `path_image.cc`, `path_image.hpp`, `path.cc`, `path.hpp`.

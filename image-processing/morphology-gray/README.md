# Morphology

C++ Morphological Image Processing program

> Based on the works of Stefan Hernandez (original project: https://github.com/StefanHernandez/Morphology).

Performs morphological operations on a FITS image using a structuring element.
The image is transformed to binary using either the median or average of the pixels.
It is assumed the image is two-dimensional.

**Important note:**
This program uses the [CFITSIO](https://heasarc.gsfc.nasa.gov/fitsio/) library to manage FITS images.

## Compilation

Within the project folder, where the Makefile is located, execute:
```bash
make template
make morph
```

## Usage

Execute the program typing:
```bash
./morphology <fits_file> <se_file> <output_file> <operation> [threshold_type]
```
Arguments:
  - `fits_file`: The input FITS file.
  - `se_file`: The structuring element file.
  - `output_file`: The output FITS file to be created.
  - `operation`: The morphological operation to perform (single letter).
Options: (e)rosion, (d)ilation, (o)pening, (c)losing.
  - `threshold_type`: The threshold to convert the data to binary (optional).
Options: (m)edian, (a)verage. Default is median.

### Structuring element format

The `se_file` must have the following structure:
- The first line indicates the shape of the SE.
```
number_of_rows number_of_columns min_value max_value
```

- The second line specifies the origin point of the SE:
```
origin_row origin_column
```

- The next lines are the actual structuting element, where the cells are separated by a single space.
```
cell_00 cell_01 cell_02 ...
cell_10 cell_11 cell_12 ...
...
```

#### Example

```
3 3 0 1
1 1
0 1 0
1 1 1
0 1 0
```

This is a 3 by 3 SE where the lowest value is 0 and the maximum is 1, also the centre of the SE is the cell (1, 1).

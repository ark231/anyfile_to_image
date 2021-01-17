# anyfile_to_image
## make pnm formatted image file from input file
### how to use
```
picgen [options] input_filepath output_filepath
```
#### options
|option|description|
|:---|:---|
|-h,--help|show help and exit|
|-X, --X_ratio NUM             |x ratio of output image|
|-Y, --Y_ratio NUM             |y ratio of output image|
|-x, --X_absolute NUM          |absolute x size (pixel)|
|-y, --Y_absolute NUM          |absolute y size (pixel)|
|-m, --mode (MONO\|GRAY\|COLOR)  |color mode of output image|
|--no_extension                |do not add extension|
### 
This program generates pnm image file whose data is the input file.
What this program actually does is to calculate the number of pixel in x axis and y axis, make pnm header using that data, copy the input file , and if the input file is too short, fill with 0.

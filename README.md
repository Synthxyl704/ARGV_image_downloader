
# ARGV image downloader (aka ARGVTD)
A fun C project I wrote for extending my knowledge in network programming and client URL handling(s) stuff.
Very, VERY easy to use terminal based image downloader using argument vector(s) straight into your required directory.

## ARGVTD working features and manual guide
- Terminal based imagery downloading straight into your `FILE*` set path directory. <br>
- Custom image resolution set from the terminal itself [WIDTHxHEIGHT]
- Grayscaling option
- Multi file extension support
- (to be added) | Raster -> Vector conversions
- (to be added) | Custom file extensions

## Linux/Terminal running commands and important notice[s]

Clone the project using this command onto bash:
```bash
git clone https://github.com/Synthxyl704/ARGV_image_downloader
```

Linux terminal:
```
-> cd <directory_of_storage>
-> gcc isoimg.c curl_utils.c magickwand.c -o isoimg `pkg-config --cflags --libs MagickWand` -lcurl
-> ./isoimg '<link_to_image>' <WIDTHxHEIGHT>
```

ARGVTD stand error notational expository:
```
// follows my ARX compiler error format created by me for readability and easier intrinsic debugging
| [:] program standard                  |


| [::] program + argv/user standard     | 
| [:::] amelioration/debug tweak string |       
```

## Definitely not FAQs again

#### Why should I use your project instead of just downloading from the site itself?

I plan on:
-> allowing raster types -> SVG/vector types conversion <br>
-> multiple file extension types flexibility, etc. from the terminal itself instead of manual configuration <br>
-> ./more <br>

Will be pretty handy for you if you wish to: <br>
-> not go through manual downloading from sites <br>
-> immediately download the image you want to a set resolution you wish from terminal itself <br>
-> not waste your time configuring extension names (yes every second counts) <br>

#### Why is it only partially complete?

I started this project on 18th June as a means to extend my knowledge on basic client URL handling and network programming during my 1st year in university and to prepare for my next subjects.
Suggestions and stuff is always recommended, please do send me those, if I am alive I will definitely read and reply to your suggestion(s)/comment(s)/note(s).

## Log for bug fixes - 5/7/25

-> Custom `vSTAT()` subsitution for `printf();` <br>
-> Increased readablity of code <br>
-> Added support for `argv[4]` which parses dot extensions of graphic files more easily as per user requirement <br>
-> `argv[3]` can now parse 2 string arguments for grayscaling, parse `GSC` for grayscaling on your img and `GSC=NULL` for turning it off <br>

<hr>

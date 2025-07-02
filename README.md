
# ARGV image downloader (aka ARGVTD)
A fun C project I wrote for extending my knowledge in network programming and client URL handling(s) stuff.
Very, VERY easy to use terminal based image downloader using argument vector(s) straight into your required directory.

### WARNING! - Please be cautious about what imagef you install and verify credibility of the site before download conduction.

## Linux/Terminal running commands and important notice[s]

Clone the project using this command onto bash:

```bash
git clone https://github.com/Synthxyl704/ARGV_image_downloader
```

!IMPORTANT!

Remember to change the file pointer to the actual directory you want to store the image to!

```bash
 FILE *filePointer = "~./example/directory/here"
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
| [:] program standard | [::] program + argv/user standard | [:::] amelioration/debug tweak string       
```

## Definitely not FAQs again

#### Can it support image rescales?

Yes, ARGVTD will store the original resolution and the modified resolution of the image to the file pointer directory you have set it in.

#### Why should I use your project instead of just downloading from the site itself?

I plan on:
-> allowing image rescalings without quality loss <br>
-> multiple file extension types flexibility, etc. from the terminal itself <br>
-> ./more <br>

will be pretty handy for you if you wish to not go through the hassle of manually downloading everything then again going to another site for rescaling and stuff.

#### Why is it only partially complete?

I started this project on 18th June as a means to extend my knowledge on basic client URL handling and network programming during my 1st year in university.
Suggestions and stuff is always recommended, please do send me those, if I am alive I will definitely read and reply to your suggestion(s)/comment(s)/note(s).

## Log for bug fixes - 28-29/6/25

-> added extension support for jpg/jpeg/png in isoimage.c<br>
-> replaced OpenCV integrations with Magickwand instead<br>
-> `argv[3]` now handles parsed permutation combinations for GSC<br>

<hr>

pdfsplit - A Mac OS X utility to split PDF into pages
=====================================================
 
### Build

----

Build it with Xcode installed then type 'make'. Though I haven't
tested myself, it should support systems above Mac OS X 10.4.

```bash
- git clone
- cd repo
- make 
```


### Run

----

```bash
usage: pdfsplit input.pdf

  - input.pdf: the path to the input pdf file.
```

Example: pdfsplit input.pdf 

Let's suppose the input.pdf has 3 pages.
It will split the file input.pdf into 3 files  

- input.1.pdf contains page 1;
- input.2.pdf contains page 2;
- input.3.pdf contains page 3;
  
[1] http://www.cs.cmu.edu/~benhdj/Mac/splitPDF.py


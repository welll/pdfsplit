// pdfsplit.c: split PDF into pages with Core Graphics framework
// A direct translation of http://www.cs.cmu.edu/~benhdj/Mac/splitPDF.py

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <ApplicationServices/ApplicationServices.h>

#define MAX_SPLIT_PARTS     1024
#define MAX_FILENAME_CHARS  256

int usage()
{
    printf("usage: pdfsplit input.pdf\n\n"
           "  - input.pdf: the path to the input pdf file.\n\n"
           "Example: pdfsplit input.pdf\n\n"
           "This will split file input.pdf into 3 files (assuming input.pdf is 3\n"
           "pages long):\n\n"
           "  - input.1.pdf contains page 1;\n"
           "  - input.2.pdf contains page 2;\n"
           "  - input.3.pdf contains page 3.\n\n");
    return 1;
}

int collectPageNums(size_t splitPageNums[], int size, size_t max)
{
    int i;

    for (i = 0; i < max; i++)
    {
        splitPageNums[i] = i+1;
       
    }

    return i;
}

void getBaseFilename(char *baseFn, int size, const char *file)
{
    int i, j, len = strlen(file);

    for (i = len - 1; i > 0 && file[i] != '.'; i--)
        ;

    for (j = 0; j < i && j < size - 1; j++)
        baseFn[j] = file[j];

    baseFn[j] = '\0';
}

void writePages(CFURLRef url, CGPDFDocumentRef inputDoc,
                size_t start, size_t end)
{
    CGContextRef writeContext = NULL;
    CGRect mediaBox;
    CGPDFPageRef page;
    size_t i;

    for (i = start; i <= end; i++)
    {
        page = CGPDFDocumentGetPage(inputDoc, i);
        mediaBox = CGPDFPageGetBoxRect(page, kCGPDFMediaBox);

        if (! writeContext)
            writeContext = CGPDFContextCreateWithURL(url, &mediaBox, NULL);

        CGPDFContextBeginPage(writeContext, NULL);
        CGContextDrawPDFPage(writeContext, page);
        CGPDFContextEndPage(writeContext);
    }

    if (writeContext)
    {
        CGPDFContextClose(writeContext);
        CGContextRelease(writeContext);
    }
}

int main(int argc, char *argv[])
{
    int error = 0, i;
    const char *inputFileName;
    char outputFn[MAX_FILENAME_CHARS], baseFn[MAX_FILENAME_CHARS];
    CGDataProviderRef inputData;
    CGPDFDocumentRef inputDoc;
    size_t maxPages, splitPageNums[MAX_SPLIT_PARTS], totalPageNums, startPageNum;

    if (argc != 2)
        return usage();

    inputFileName = argv[1];
    inputData = CGDataProviderCreateWithFilename(inputFileName);

    if (! inputData)
    {
        printf("Error: failed to open %s\n", inputFileName);
        error = -1;
        goto cleanup;
    }

    inputDoc = CGPDFDocumentCreateWithProvider(inputData);

    if (! inputDoc)
    {
        printf("Error: failed to open %s\n", inputFileName);
        error = -1;
        goto cleanup;
    }
    
    maxPages = CGPDFDocumentGetNumberOfPages(inputDoc);
    printf("%s has %lu pages\n", inputFileName, maxPages);

    totalPageNums =  collectPageNums(splitPageNums, MAX_SPLIT_PARTS, maxPages);

    if (! totalPageNums)
        goto cleanup;

    if (totalPageNums < MAX_SPLIT_PARTS &&
        splitPageNums[totalPageNums - 1] < maxPages)
        splitPageNums[totalPageNums++] = maxPages;

    getBaseFilename(baseFn, MAX_FILENAME_CHARS, basename((char *) inputFileName));

    startPageNum = 1;
    for (i = 0; i < totalPageNums; i++)
    {
        CFURLRef url;

        if (startPageNum < splitPageNums[i])
            snprintf(outputFn, MAX_FILENAME_CHARS, "%s.%lu-%lu.pdf",
                     baseFn, startPageNum, splitPageNums[i]);
        else
            snprintf(outputFn, MAX_FILENAME_CHARS, "%s.%lu.pdf",
                     baseFn, startPageNum);

        url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *) outputFn,
                                                      strlen(outputFn), false);

        printf("Writing page %lu-%lu to %s...\n",
               startPageNum, splitPageNums[i], outputFn);

        writePages(url, inputDoc, startPageNum, splitPageNums[i]);
        CFRelease(url);

        startPageNum = splitPageNums[i] + 1;
    }

cleanup:
    CGPDFDocumentRelease(inputDoc);
    CGDataProviderRelease(inputData);

    return error;
}


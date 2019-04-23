#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>

int main() {
    Pix *img = pixRead("screen.png");
    img = pixConvertRGBToGrayFast(img);
    img = pixInvert(nullptr, img);

    Pix *binarized = nullptr;
    pixSauvolaBinarize(img, 20, 1.46f, 1, nullptr, nullptr, nullptr, &binarized);
    Pix *output = pixCreate(binarized->w, binarized->h, 32);
    pixConvert1To32(output, binarized, 0xffffffff, 0x000000ff);

    auto *api = new tesseract::TessBaseAPI();
    api->Init(nullptr, "eng");
    api->SetImage(binarized);

    Box scan = {1600, 20, 250, 400, 0};

    api->SetRectangle(scan.x, scan.y, scan.w, scan.h);

    BOX *s = boxCreate(scan.x, scan.y, scan.w, scan.h);
    pixRenderBoxBlend(output, s, 3, 0, 0xff, 0xff, 0.5f);
    boxDestroy(&s);

    api->Recognize(nullptr);

    tesseract::ResultIterator *ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    uint8_t col = 0;
    if (ri) {
        do {
            const char *word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);

            if (conf > 50.0f) {
                printf("%10s\t%.2f\tBB: [%5d, %5d, %5d, %5d]\n",
                       word, conf, x1, y1, x2, y2);
               BOX *b = boxCreate(x1, y1, x2 - x1, y2 - y1);
               pixRenderBoxBlend(output, b, 3, 0xff, col % 128, col % 128, 0.5f);
               boxDestroy(&b);
               col++;
            }
            delete[] word;
        } while (ri->Next(level));
    }

    api->End();

    pixWrite("output.png", output, IFF_PNG);

    return 0;
}
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include "device.hpp"
#include "util.hpp"
#include "bot.hpp"
#include <windows.h>
#include <io.h>
#include <opencv2/opencv.hpp>

int opencv_main();

int tesseract_main();

int main(int argc, char *argv[]) {
    //return opencv_main();

    PrintTitle();

    if (!IsAdbAvailable()) {
        Error("adb was not found!");
    }

    if (!IsDeviceConnected()) {
        Error("no device connected!");
    }

    PrintAdbDevice();

    if (DeviceUnlockedAndAwake()) {
        StartCoC();
    } else {
        Error("please unlock device!");
    }

    Bot bot;
    bot.requestBaseCenter();

    while (true) {
        if (!bot.process()) {
            std::cerr << "Error: bot couldn't correctly process screenshot, retrying..." << std::endl;
        }
    }

    return 0;
}

int opencv_main() {
    cv::Mat ref = cv::imread("screen.png");
    cv::resize(ref, ref, {1920, 1080});

    cv::Mat tpl = cv::imread("train_green.png");
    if (ref.empty() || tpl.empty()) {
        std::cout << "Error reading file(s)!" << std::endl;
        return -1;
    }

    //cv::imshow("screen", ref);
    //cv::imshow("troops", tpl);

    cv::Mat res_32f(ref.rows - tpl.rows + 1, ref.cols - tpl.cols + 1, CV_32FC1);
    cv::matchTemplate(ref, tpl, res_32f, cv::TM_CCOEFF_NORMED);

    cv::Mat res;
    res_32f.convertTo(res, CV_8U, 255.0);
    cv::imshow("result", res);

    int size = ((tpl.cols + tpl.rows) / 4) * 2 + 1; //force size to be odd
    //cv::adaptiveThreshold(res, res, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C , cv::THRESH_BINARY, size, -128);
    cv::threshold(res, res, 240.0, 255.0, cv::THRESH_TOZERO);
    cv::adaptiveThreshold(res, res, 240, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, size, -128);
    cv::imshow("result_thresh", res);

    while (true) {
        double minval, maxval, threshold = 0.9;
        cv::Point minloc, maxloc;
        cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

        if (maxval >= threshold * 255) {
            cv::rectangle(ref, maxloc, cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows), CV_RGB(0, 10, 255), 2);
            cv::floodFill(res, maxloc, 0); //mark drawn blob
            std::cout << maxval << " " << cv::Rect(maxloc, cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows))
                      << std::endl;
        } else
            break;
    }

    cv::imshow("final", ref);
    cv::waitKey(0);

    return 0;
}


int tesseract_main() {
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
#pragma once
#include <string>
#include <utility>

namespace ddpi {

struct ROI_t {
    ROI_t() {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }

    ROI_t(int X, int Y, int w, int h) {
        x = X;
        y = Y;
        width = w;
        height = h;
    }

    ROI_t(const ROI_t &obj) {
        x = obj.x;
        y = obj.y;
        width = obj.width;
        height = obj.height;
    }

    int x;
    int y;
    int width;
    int height;
};

struct Blob_t {
    Blob_t() {
        x = 0.0f;
        y = 0.0f;
        radius = 0.0f;
        score = 0.0f;
        I = 0.0f;
    }

    Blob_t(float X, float Y, float r, float intensity, float s, ROI_t box) {
        x = X;
        y = Y;
        I = intensity;
        radius = r;
        score = s;
        bbox = box;
    }

    Blob_t(const Blob_t &obj) {
        x = obj.x;
        y = obj.y;
        radius = obj.radius;
        I = obj.I;
        score = obj.score;
        bbox = obj.bbox;
    }

    float x;
    float y;
    float radius;
    float I;
    float score;
    ROI_t bbox;
};

struct DDPIdata_t {
    DDPIdata_t() { tag = 0; }

    DDPIdata_t(const Blob_t &P1, const Blob_t &P4, int TAG) {
        p1 = P1;
        p4 = P4;
        tag = TAG;
    }

    DDPIdata_t(const DDPIdata_t &obj) {
        p1 = obj.p1;
        p4 = obj.p4;
        tag = obj.tag;
    }

    Blob_t p1;
    Blob_t p4;
    int tag;
};

};
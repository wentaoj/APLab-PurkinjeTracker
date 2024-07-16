#include "functional"
#include "iostream"
#include "utils.hpp"
#include "tracker.hpp"

using namespace std;

int main()
{
    // string src = "recording.avi";
    // string src = "211122_optotype_17-3mm_867mm_SM.avi";
    string src = "sample.mp4";
    DoHTracker tracker(50, 3, 20, 0.001, 0);
    timingdecorator([&]()
                    { tracker.track(src); });
    return 0;
}

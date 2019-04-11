#include <sigslot/signal.hpp>
#include <chrono>
#include <cassert>

using namespace sigslot;

void test_signal_performance() {
    using Clock                = std::chrono::high_resolution_clock;
    using TimePoint            = std::chrono::time_point<Clock>;

    constexpr int numConnections = 1000;
    double referenceSignalTimeMsec = 0.;
    sigslot::signal<> sig;
    {
        std::vector<scoped_connection> connections;
        connections.reserve(numConnections);
        for (int i = 0; i < numConnections; i++) {
            connections.emplace_back(sig.connect([]{}));
        }

        // Measure first signal time as reference
        const TimePoint begin = Clock::now();
        sig();
        referenceSignalTimeMsec = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - begin).count() / 1000.;
    }

    // Measure signal after all slot were disconnected
    const TimePoint begin = Clock::now();
    sig();
    const double afterDisconnectSignalTimeMsec = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - begin).count() / 1000.;

    // Ensure that the signal cost is not > at 10%
    const auto acceptableDelta = referenceSignalTimeMsec * 0.1;
    assert(afterDisconnectSignalTimeMsec < referenceSignalTimeMsec + acceptableDelta &&
           afterDisconnectSignalTimeMsec > referenceSignalTimeMsec - acceptableDelta);
}

int main() {
    test_signal_performance();
    return 0;
}
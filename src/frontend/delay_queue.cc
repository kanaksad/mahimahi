/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <limits>
#include <random>
#include <chrono>

#include "delay_queue.hh"
#include "timestamp.hh"

using namespace std;

void DelayQueue::read_packet( const string & contents )
{
    if (delay_ms_ == 42) {
        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> uni(0, 20);

        auto random_delay_ms = uni(rng);
        packet_queue_.emplace( timestamp() + random_delay_ms, contents );
    } else {
        packet_queue_.emplace( timestamp() + delay_ms_, contents );
    }
}

void DelayQueue::write_packets( FileDescriptor & fd )
{
    while ( (!packet_queue_.empty())
            && (packet_queue_.front().first <= timestamp()) ) {
        fd.write( packet_queue_.front().second );
        packet_queue_.pop();
    }
}

unsigned int DelayQueue::wait_time( void ) const
{
    if ( packet_queue_.empty() ) {
        return numeric_limits<uint16_t>::max();
    }

    const auto now = timestamp();

    if ( packet_queue_.front().first <= now ) {
        return 0;
    } else {
        return packet_queue_.front().first - now;
    }
}

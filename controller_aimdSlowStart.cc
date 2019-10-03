#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

void GetDatagram(bool TO_Datagram);
bool GetTimeout();
bool Glob_Timeout = NULL;
unsigned int window_global = 2;

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{
  /* Default: fixed window size of 100 outstanding datagrams */
  unsigned int the_window_size = window_global;
    //cout << "win size is bawah : " << the_window_size << Glob_Timeout;
    /*while (GetTimeout(Glob_Timeout) == false) {
     the_window_size = the_window_size + 0.2;   
    cout << "Datagram and size = " << GetTimeout(Glob_Timeout) <<";"<< the_window_size;   
    }*/
       /*do{
        if(GetTimeout() == false && the_window_size <= CongestionThreshold){
         the_window_size++;
         cout << "win size is atas : " << the_window_size;
        } else if (GetTimeout() == true && the_window_size >= 2){
            the_window_size = the_window_size * 1/2;
            cout << " Win size is (bawah) : " << the_window_size;       
            }
       }
        while (GetTimeout() == true);*/
    cout << "win size is atas : " << the_window_size;
    if (GetTimeout() == false && window_global <= 50){
        //cout << "win size is atas : " << the_window_size;
        window_global = window_global + 1;
            //if(GetTimeout() == true) break;
    } else
    if  (GetTimeout() == true && window_global <= 50){
        //cout << "FOR BAWAH : " << the_window_size;
        window_global = window_global * 1/2;
            //if(GetTimeout() == false) break;       

    };
  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
     << " window size is " << the_window_size << endl;
  }
 return window_global;
 return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
                    /* of the sent datagram */
                    const uint64_t send_timestamp,
                                    /* in milliseconds */
                    const bool after_timeout
                    /* datagram was sent because of a timeout */ )
{
  /* Default: take no action */
    GetDatagram(after_timeout);
  if ( debug_ ) {
    cerr << "At time " << send_timestamp
     << " sent datagram " << sequence_number << " (timeout = " << after_timeout << ")\n";
  }
}

void GetDatagram(bool TO_Datagram) {
    Glob_Timeout = TO_Datagram;
    GetTimeout();
}

bool GetTimeout() {
    return Glob_Timeout;
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
                   /* what sequence number was acknowledged */
                   const uint64_t send_timestamp_acked,
                   /* when the acknowledged datagram was sent (sender's clock) */
                   const uint64_t recv_timestamp_acked,
                   /* when the acknowledged datagram was received (receiver's clock)*/
                   const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
     << " received ack for datagram " << sequence_number_acked
     << " (send @ time " << send_timestamp_acked
     << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
     << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms()
{
  return 1000; /* timeout of one second */
}
#include <iostream>
#include <algorithm>
#include <vector>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

void GetDatagram(bool TO_Datagram);
void CalcBW_RTT_ACK(uint64_t recv_ack, uint64_t seq_acked);
unsigned int GetThreshold();
bool GetTimeout();

bool Glob_Timeout;
unsigned int window_global = 2;
int64_t EstRTT;
vector<uint64_t> acked_seq;
uint64_t DUPACK;
bool DUPACK_;
uint64_t accounted_for;
uint64_t Curr_BW;
uint64_t LastSample_BW;
uint64_t Last_BW;

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{
  /* Default: fixed window size of 100 outstanding datagrams */
  unsigned int the_window_size = window_global;
    unsigned int Threshold = GetThreshold();
    //cout << "LAST BW "<< Last_BW;
    //cout << "LAST RTT "<< EstRTT;
    if (GetTimeout() == false && window_global <= 50){
        window_global += 1;
    };if  (DUPACK_ == true){
        Threshold = (Last_BW*EstRTT)/DUPACK;
        if (window_global > Threshold){
        window_global = Threshold;
        };
    };
    if  (GetTimeout() == true){
        cout << " TEST TIMEOUT ";
        Threshold = (Last_BW*EstRTT)/DUPACK;
        if (Threshold < 2){
        Threshold = 2;
        };
        window_global = 2;
    };

    /*if (GetTimeout() == true){
        Threshold = (Last_BW*EstRTT)/2;
    }
    else if (window_global > Threshold){
        window_global = Threshold;
    }
    else {
        window_global += 1;
    }*/

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
    CalcBW_RTT_ACK(timestamp_ack_received,sequence_number_acked);
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

unsigned int GetThreshold(){
    return std::max(int(2*DUPACK), int(Curr_BW * EstRTT));
}

void CalcBW_RTT_ACK(uint64_t recv_ack, uint64_t seq_acked) {
    uint64_t sampleBWE;
    int n;
    uint64_t alpha = 0.9;
    uint64_t RTT = recv_ack;
    uint64_t last_ack;
    uint64_t cumul_ack;
    uint64_t curr_ack;

    /* Calculate BW HERE */   
    Curr_BW = seq_acked * DUPACK / RTT;
    sampleBWE = Curr_BW;
    Curr_BW = (alpha * Last_BW) + ((1 - alpha) * ((sampleBWE + LastSample_BW) / 2));
    LastSample_BW = sampleBWE;
    Last_BW = Curr_BW;
    EstRTT = RTT;

    /*CALCULATE ACK HERE*/
    acked_seq.push_back(seq_acked);
    DUPACK_ = false;
    n = acked_seq.size();
    curr_ack = acked_seq[n];
    last_ack = acked_seq[n-1];
    cumul_ack = curr_ack - last_ack;
    //cout << " TEST ACC FOR "<< cumul_ack;
    if (cumul_ack == 0){
        cout << " TEST IF CUMUL ACK 0";
        accounted_for += 1;
        cumul_ack = 1;
        DUPACK_ = true;
    };
    if (cumul_ack > 1){
        if (accounted_for >= cumul_ack) {
          cout << " TEST ACC IF ACC > CUMUL ";
          accounted_for = accounted_for - cumul_ack;
          cumul_ack = 1;
          DUPACK_ = true;
        } else if (accounted_for < cumul_ack){
            //cout << " TEST ACC FOR < cumul_ack";
             cumul_ack = cumul_ack - accounted_for;
             accounted_for = 0;   
        }
    };
    last_ack = curr_ack;
    DUPACK = cumul_ack;
    //cout << " TEST ACC FOR "<< DUPACK;
}

void GetDatagram(bool TO_Datagram) {
    Glob_Timeout = TO_Datagram;
    GetTimeout();
}

bool GetTimeout() {
    return Glob_Timeout;
}
#include<iostream>
#include"usekey.h"
#include<array>
#include"aes.h"
extern key my_key;
std::array<uint8_t , 16> add_key(std::array<uint8_t , 16> state , int r){
  
  for(int i =0 ; i<16 ; i++){
    state[i] ^= my_key.round_key[r][i]; 
  }

  return state;

}

std::array<uint8_t , 16> subbytes(std::array<uint8_t , 16 > state){
  for(int i =0 ; i<16; i++){
    state[i]=my_key.sbox[state[i]];
  }
  return state;

}

std::array<uint8_t , 16> shiftRow(std::array<uint8_t , 16> state){
    std::array<uint8_t , 16> temp = state;

    // Row 0 (indices 0, 4, 8, 12) -> No shift

    // Row 1 (indices 1, 5, 9, 13) -> Shift left by 1
    state[1]  = temp[5];
    state[5]  = temp[9];
    state[9]  = temp[13];
    state[13] = temp[1];

    // Row 2 (indices 2, 6, 10, 14) -> Shift left by 2
    state[2]  = temp[10];
    state[6]  = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];

    // Row 3 (indices 3, 7, 11, 15) -> Shift left by 3
    state[3]  = temp[15];
    state[7]  = temp[3];
    state[11] = temp[7];
    state[15] = temp[11];

    return state;
}

//from here it is the finite field arithmatic
uint8_t multy_by_2(uint8_t x){
  bool temp = x&0x80;
  x=x<<1;
  if(temp){
    x^=0x1B;

  }
  return x;
}
uint8_t multy_by_3(uint8_t x){
  return multy_by_2(x)^x;
}

std::array<uint8_t,16> mixcolumn(std::array<uint8_t , 16> state){
  std::array<uint8_t , 16> b;

  for(int i=0 ; i<13;i+=4){

  b[0+i] = multy_by_2(state[0+i])^multy_by_3(state[1+i])^state[2+i]^state[3+i];
  b[1+i] = state[0+i]^ multy_by_2(state[1+i])^multy_by_3(state[2+i])^state[3+i];
  b[2+i] = state[0+i]^state[1+i]^multy_by_2(state[2+i])^multy_by_3(state[3+i]);
  b[3+i] = multy_by_3(state[0+i])^state[1+i]^state[2+i]^multy_by_2(state[3+i]);
  }

  return b;

}

void encript(std::array<uint8_t , 16>& state){

  int round=0;
  //round 0
  state = add_key(state , round);
  round++;
  //round 1-9
  while(true){
  state=subbytes(state);
  state=shiftRow(state);
  state=mixcolumn(state);
  state=add_key(state , round);

  round++;
    if(round==10){
      break;
    }
  }
  //round 10
  state=subbytes(state);
  state=shiftRow(state);
  state=add_key(state ,round);

  
}



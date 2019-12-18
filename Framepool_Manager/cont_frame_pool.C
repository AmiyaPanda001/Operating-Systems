/*
 File: ContFramePool.C
 
 Author: Amiya Rajan Panda
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

/* 00 - unreserved, 11 - reserved, 01 - info frame, 10 - head of sequence */

//initializing static variables
ContFramePool* ContFramePool::framepool_list[100];
unsigned int ContFramePool::framepool_index = 0;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{

    //adding framepool to list
    if(framepool_index != 0) framepool_index++;
    framepool_list[framepool_index] == this;    
	
    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;


    // If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }

    // Everything ok. Proceed to mark all bits in the bitmap
    for(int i=0; i*4 < _n_frames; i++) {
        bitmap[i] = 0x00;
    }

    _n_info_frames = _n_frames/(2*4096) + 1;

    // Mark the first frame as being used if it is being used
    if(_info_frame_no == 0) {
	bitmap[0] |= 0x40; 
	 nFreeFrames--;}
       }
    
    //first frame from which the allocation starts
    //first_frame_byte = _n_info_frames/4;
    //first_frame_bit = _n_info_frames/4;

    //Console::puts("Frame Pool initialized\n");

//    assert(false);


unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    int N_frames = _n_frames;
    int i = 0,j=0,k,l, frame_no = 0, n_frame_test = 0, test_i = 0, found = 0;

    //Console::puts("Frame Pool search initialized\n");
    //searching for n free frames-----------------------------------------------------------------
	while(bitmap[i] != 0x0)i++;
	
     //-----------------------------------------------------------------------------------
       //Console::puts("Frame Pool search complete\n");
	//found = 1;
       if(i >= (this->nframes*4)) return 0;
     Console::puts("j \t");Console::puti(j);


    //allocating frames--------------------------------------------------------------------
   // Console::puts("Frame Pool allocation initialized\n");
    frame_no += (i)*4;

    if (j == 0) {
	
	bitmap[i] |= 0x80;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}


    if (j == 1) {
	frame_no += 1;
	bitmap[i] |= 0x20;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}
	
	if (j == 2) {
	frame_no += 2;
	bitmap[i] |= 0x08;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}
	
	if (j == 3) {
	frame_no += 3;
	bitmap[i] |= 0x02;
	nFreeFrames--;
	N_frames-- ;
	i++;
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}
	
	

    return frame_no;
    //assert(false);
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    int Base_frame_no = _base_frame_no;
    int N_frames = _n_frames;
    int i = 0,j = 0,k,l, frame_no = 0;
    i = Base_frame_no/4;
    j = Base_frame_no%4;

    if (j== 0) {
	bitmap[i] |= 0x80;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}

    else if (j== 1) {
	bitmap[i] |= 0x20;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;}
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}
	
      else if (j == 2) {

	bitmap[i] |= 0x08;
	nFreeFrames--;
	N_frames-- ;
	
	if(N_frames > 0){ bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ; i++;}
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}
	
	else if (j == 3) {
	bitmap[i] |= 0x02;
	nFreeFrames--;
	N_frames-- ;
	i++;
	
	while(N_frames > 0){
	   bitmap[i] |= 0xc0;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;
	   
	   bitmap[i] |= 0x30;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x0c;nFreeFrames--;N_frames-- ;
	   if(N_frames == 0) break;

	   bitmap[i] |= 0x03;nFreeFrames--;N_frames-- ;i++;
	   if(N_frames == 0) break;
	}}


    //assert(false);
}


void ContFramePool::release_frames(unsigned long _first_frame_no){

	//Console::puts("de-allocation ");
	//Console::puti(_first_frame_no);Console::puts("\n");
	for(int i = 0; i < 100; i++){
		//Console::puti(framepool_list[i]->base_frame_no);Console::puts("\t");
		if((_first_frame_no >= framepool_list[i]->base_frame_no)&&
		(_first_frame_no <= (framepool_list[i]->base_frame_no + framepool_list[i]->nframes)))

						framepool_list[i]->pool_release_frames(_first_frame_no);
	}

}


void ContFramePool::pool_release_frames(unsigned long _first_frame_no)
{
    //Console::puts("de-allocation ");
    int First_frame_no = _first_frame_no;
    int i = 0,j = 0,k,l, frame_no = 0;
	
    i = First_frame_no/4;
    j = First_frame_no%4;

    if (j == 0) {	
	bitmap[i] &= 0x3f;
	nFreeFrames++;
	
	if(bitmap[i] & 0x30 == 0x30){ bitmap[i] &= 0xcf;nFreeFrames++;}
	if(bitmap[i] & 0x0c == 0x0c){ bitmap[i] &= 0xf3;nFreeFrames++;}
	if(bitmap[i] & 0x03 == 0x03){ bitmap[i] &= 0xfc;nFreeFrames++; i++;}
	
	while(bitmap[i] & 0xc0 == 0xc0){
	   bitmap[i] &= 0x3f;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;
	   
	   bitmap[i] &= 0xcf;nFreeFrames++;
	   if(bitmap[i] & 0x0c != 0x0c) break;

	   bitmap[i] &= 0xf3;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;

	   bitmap[i] &= 0xfc;nFreeFrames++;i++;
	   //if(N_frames == 0) break;
	}}

    else if (j == 1) {	
	bitmap[i] &= 0xcf;
	nFreeFrames++;
	
	if(bitmap[i] & 0x0c == 0x0c){ bitmap[i] &= 0xf3;nFreeFrames++;}
	if(bitmap[i] & 0x03 == 0x03){ bitmap[i] &= 0xfc;nFreeFrames++; i++;}
	
	while(bitmap[i] & 0xc0 == 0xc0){
	   bitmap[i] &= 0x3f;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;
	   
	   bitmap[i] &= 0xcf;nFreeFrames++;
	   if(bitmap[i] & 0x0c != 0x0c) break;

	   bitmap[i] &= 0xf3;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;

	   bitmap[i] &= 0xfc;nFreeFrames++;i++;
	   //if(N_frames == 0) break;
	}}
	
	else if (j == 2) {
	bitmap[i] &= 0xf3;
	nFreeFrames++;
	
	if(bitmap[i] & 0x03 == 0x03){ bitmap[i] &= 0xfc;nFreeFrames++; i++;}
	
	while(bitmap[i] & 0xc0 == 0xc0){
	   bitmap[i] &= 0x3f;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;
	   
	   bitmap[i] &= 0xcf;nFreeFrames++;
	   if(bitmap[i] & 0x0c != 0x0c) break;

	   bitmap[i] &= 0xf3;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;

	   bitmap[i] &= 0xfc;nFreeFrames++;i++;
	   //if(N_frames == 0) break;
	}}
	
	else if (j == 3) {
	bitmap[i] &= 0xfc;
	nFreeFrames++;
	i++;	
	
	while(bitmap[i] & 0xc0 == 0xc0){
	   bitmap[i] &= 0x3f;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;
	   
	   bitmap[i] &= 0xcf;nFreeFrames++;
	   if(bitmap[i] & 0x0c != 0x0c) break;

	   bitmap[i] &= 0xf3;nFreeFrames++;
	   if(bitmap[i] & 0x03 != 0x03) break;

	   bitmap[i] &= 0xfc;nFreeFrames++;i++;
	   //if(N_frames == 0) break;
	}}

    //assert(false);
    
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{

    //assert(false);
}

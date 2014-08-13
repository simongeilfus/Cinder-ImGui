/*
 Muon Project
 Copyright (c) 2013-2014, Simon Geilfus - All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */


#include "Timer.h"

namespace muon {
    
    //! Constructs a default timer which is initialized as stopped
    Timer::Timer()
    : mMode( GPU ), mIsStopped( true )
    {
    }
    //! Constructs a default timer which is initialized as running unless \a startOnConstruction is false
    Timer::Timer( bool startOnConstruction )
    : mMode( GPU ), mIsStopped( true )
    {
        if( mMode == CPU ){
            mCpuTimer = ci::Timer( startOnConstruction );
        }
    }
    
    void Timer::setMode( int mode )
    {
        mMode = mode;
    }
    //! Returns Timer mode
    int Timer::getMode()
    {
        return mMode;
    }
    //! Toggle Timer mode
    void Timer::toggleMode()
    {
        mMode = !mMode;
    }
    
    //! Begins timing
    void Timer::start()
    {
        if( mMode == CPU ){
            mCpuTimer.start();
        }
        else {
            if( !mGpuTimer[0] ){
                glGenQueries( 1, mGpuTimer );
            }
            
            glBeginQuery( GL_TIME_ELAPSED_EXT, mGpuTimer[0] );
        }
        mIsStopped = false;
    }
    
    //! Returns the elapsed seconds if the timer is running, or the total time between calls to start() and stop() if it is stopped.
    double Timer::getSeconds()
    {
        if( mMode == CPU ){
            return mCpuTimer.getSeconds();
        }
        else {
            if( !mIsStopped ){
                stop();
            }
            return mElapsedSeconds;
        }
    }
    //! Ends timing
    void Timer::stop()
    {
        if( mMode == CPU ){
            mCpuTimer.stop();
            mElapsedSeconds = mCpuTimer.getSeconds();
        }
        else {
            glEndQuery( GL_TIME_ELAPSED_EXT );
            
            GLint64EXT elapsedNanoSeconds = 0;
            glGetQueryObjecti64vEXT( mGpuTimer[0], GL_QUERY_RESULT, &elapsedNanoSeconds);
            mElapsedSeconds = elapsedNanoSeconds * 0.000000001;
        }
		mIsStopped = true;
    }
}
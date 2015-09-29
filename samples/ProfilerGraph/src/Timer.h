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

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Timer.h"

namespace muon {
    
    
    class Timer {
    public:
        enum Modes {
            CPU,
            GPU
        };
        
        //! Constructs a default timer which is initialized as stopped
        Timer();
        //! Constructs a default timer which is initialized as running unless \a startOnConstruction is false
        Timer( bool startOnConstruction );
        
        //! Sets Timer mode
        void setMode( int mode );
        //! Returns Timer mode
        int getMode();
        //! Toggle Timer mode
        void toggleMode();
        
        //! Begins timing
        void	start();
        //! Returns the elapsed seconds if the timer is running, or the total time between calls to start() and stop() if it is stopped.
        double	getSeconds();
        //! Ends timing
        void	stop();
        
        //! Returns whether the timer is currently running
        bool	isStopped() const { return mIsStopped; }
        
    private:
        int         mMode;
        bool        mIsStopped;
        double      mElapsedSeconds;
        
        ci::Timer   mCpuTimer;
        GLuint      mGpuTimer[1];
        
    };
    
    
}
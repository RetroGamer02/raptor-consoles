package com.raptor.skynettx;

import org.libsdl.app.SDLActivity;

public class RaptorSDL3Activity extends SDLActivity
{
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL3",
                "raptor"
        };
    }
}

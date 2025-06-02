package org.graphgui;

import com.sun.jna.Library;
import com.sun.jna.Native;


public interface GraphLib extends Library {//functions from interface.c to java
    GraphLib INSTANCE = Native.load("graphlib", GraphLib.class);

    void fromFile(String inputFilePath, int k, double precision,String outputFilePath);
    void fromTests(int option, int n, double connectionProbability, int k, double precision, String outputFilePath);
    void fromUserInput(String inputFile, int k, double precision, String outputFilePath);
}

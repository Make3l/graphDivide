package org.graphgui;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class FileUtils {
    public static String fileToString(String filePath){//converts content from file to String
        String input;
        try{
            input= Files.readString(Path.of(filePath));
        }catch (IOException e) {
            input="Error occured during file loading: "+e.getMessage();
        }
        return input;
    }
}

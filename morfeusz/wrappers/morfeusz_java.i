%typemap(javaimports) Morfeusz %{
import java.io.IOException;
%}

%javaexception("java.io.IOException") Morfeusz::setAnalyzerDictionary {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%javaexception("java.io.IOException") Morfeusz::setGeneratorDictionary {
    try {
        $action
    }
    catch(FileFormatException & e) {
        jclass clazz = jenv->FindClass("java/io/IOException");
        jenv->ThrowNew(clazz, "Invalid file format");
        return $null;
    }
}

%include "enums.swg"

/* Force the generated Java code to use the C enum values rather than making a JNI call */
%javaconst(1);

%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("jmorfeusz");
  }
%}
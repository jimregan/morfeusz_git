/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package pl.waw.ipipan.morfeusz;

public class MorphInterpretation {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected MorphInterpretation(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(MorphInterpretation obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        MorfeuszWrapperJNI.delete_MorphInterpretation(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public int getStartNode() {
    return MorfeuszWrapperJNI.MorphInterpretation_getStartNode(swigCPtr, this);
  }

  public int getEndNode() {
    return MorfeuszWrapperJNI.MorphInterpretation_getEndNode(swigCPtr, this);
  }

  public String getOrth() {
    return MorfeuszWrapperJNI.MorphInterpretation_getOrth(swigCPtr, this);
  }

  public String getLemma() {
    return MorfeuszWrapperJNI.MorphInterpretation_getLemma(swigCPtr, this);
  }

  public int getTagnum() {
    return MorfeuszWrapperJNI.MorphInterpretation_getTagnum(swigCPtr, this);
  }

  public int getNamenum() {
    return MorfeuszWrapperJNI.MorphInterpretation_getNamenum(swigCPtr, this);
  }

  public String getTag() {
    return MorfeuszWrapperJNI.MorphInterpretation_getTag(swigCPtr, this);
  }

  public String getName() {
    return MorfeuszWrapperJNI.MorphInterpretation_getName(swigCPtr, this);
  }

}

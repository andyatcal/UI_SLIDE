# The following Tcl Block defines some basic math functions
#{

proc DegToRad { degrees } {
  global SLF_PI
  return [expr $degrees * $SLF_PI / 180]
}

proc RadToDeg { radians } {
  global SLF_PI
  return [expr $radians * 180 / $SLF_PI]
}

proc Min { a b } {
  set min $a

  if { $b < $a } {
    set min $b
  }

  return $min
}

proc Max { a b } {
  set max $a

  if { $b > $a } {
    set max $b
  }

  return $max
}

proc Oscillate {} {
  global SLF_TIME
  return [expr (cos($SLF_TIME) + 1) / 2]
}


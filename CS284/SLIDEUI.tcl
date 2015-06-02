source SLFCONSTS.tcl
source MATH.tcl

proc RGBScaleToByte { value } {
    return [expr int($value * 255)]
}

proc CBRGBColor { root name value } {
    set varR "[subst $name]_r"
    set varG "[subst $name]_g"
    set varB "[subst $name]_b"

    global $varR $varG $varB
    set byteR [RGBScaleToByte [subst $$varR]]
    set byteG [RGBScaleToByte [subst $$varG]]
    set byteB [RGBScaleToByte [subst $$varB]]

    $root.sample conf -bg [format \#%02x%02x%02x \
	    $byteR $byteG $byteB]
}

#fieldList is a list of SLIDE constants { field0 field1 ... }
#returns { {field0 value0} ... }
proc CreateSLIDEEnumList { fieldList } {
    set enumList {}

    foreach field $fieldList {
	global $field
	lappend enumList "[subst $field] [subst $$field]"
    }

    return $enumList
}

#list is an enumerated list { {field0 value0} ... }
proc CreateRadioButtonFrame { name parent this text default enumList } {
    set root $parent.$this

    frame $root -relief raised -bd 2

    set varThis "[subst $name]_[subst $this]"

    global $varThis

    set $varThis $default

    label $root.title -text $text
    pack $root.title -side top -fill x

    foreach choice $enumList {
	set field [eval {lindex $choice 0}]
	set value [eval {lindex $choice 1}]
	set varButton $root.slf$field

	radiobutton $varButton -text $field -variable $varThis \
		-value $value -anchor w
	pack $varButton -side top -fill x
    }

    return $root
}

proc CalculateTickInterval { min max tickints } {
    return [expr ($max - $min)/$tickints]
}

proc CreateScale { name parent this text default min max resolution tickints orient } {
    set root $parent.$this

    set varThis "[subst $name]_[subst $this]"

    global $varThis

    set $varThis $default

    set tickinterval [CalculateTickInterval $min $max $tickints]

    scale $root -label $text -from $min -to $max  \
	    -resolution $resolution -tickinterval $tickinterval \
	    -orient $orient -var $varThis

    return $root
}

proc CreateScaleCmd { name parent this text default min max resolution tickints orient cmd } {
    set root $parent.$this

    set varThis "[subst $name]_[subst $this]"

    global $varThis

    set $varThis $default

    set tickinterval [CalculateTickInterval $min $max $tickints]

    scale $root -label $text -from $min -to $max  \
	    -resolution $resolution -tickinterval $tickinterval \
	    -orient $orient -var $varThis -command $cmd

    return $root
}

proc CreateDoubleScale { name parent this thisA thisB textA textB min defaultA defaultB max resolution tickints orient } {
    set root $parent.$this
    set rootA $root.$thisA
    set rootB $root.$thisB

    set varThisA "[subst $name]_[subst $thisA]"
    set varThisB "[subst $name]_[subst $thisB]"

    global $varThisA $varThisB 

    set $varThisA $defaultA
    set $varThisB $defaultB

    set defaultA [expr $defaultA + abs($resolution)]
    set defaultB [expr $defaultB - abs($resolution)]

    set tickintervalA [CalculateTickInterval $min $defaultB $tickints]
    set tickintervalB [CalculateTickInterval $defaultA $max $tickints]

    frame $root

    scale $rootA -label $textA -from $min -to $defaultB  \
	    -resolution $resolution -tickinterval $tickintervalA \
	    -orient $orient -var $varThisA -command "$rootB \
	    config -tickinterval \[CalculateTickInterval \[expr $$varThisA + abs($resolution)\] $max $tickints\] -from \[expr $$varThisA + abs($resolution)\]; expr "

    scale $rootB -label $textB -from $defaultA -to $max  \
	    -resolution $resolution -tickinterval $tickintervalB \
	    -orient $orient -var $varThisB -command "$rootA \
	    config -tickinterval \[CalculateTickInterval $min \[expr $$varThisB - abs($resolution)\] $tickints\] -to \[expr $$varThisB - abs($resolution)\]; expr "

    pack $rootA $rootB -side left -fill y

    return $root
}

proc CreateLODFrame { name parent } {
    set root $parent

    set lodList [list SLF_OFF SLF_BOUND SLF_EDGES SLF_FULL]
    set enumList [CreateSLIDEEnumList $lodList]

    global SLF_FULL

    return [CreateRadioButtonFrame $name $root lod LOD $SLF_FULL $enumList]
}

proc CreateShadingFrame { name parent } {
    set root $parent

    set shadingList [list SLF_INHERIT SLF_WIRE SLF_FLAT SLF_GOURAUD SLF_PHONG]
    set enumList [CreateSLIDEEnumList $shadingList]

    global SLF_INHERIT

    return [CreateRadioButtonFrame $name $root shading Shading $SLF_INHERIT $enumList]
}

proc CreateSolidFrame { name parent } {
    set root $parent

    set solidList [list SLF_SOLID SLF_HOLLOW]
    set enumList [CreateSLIDEEnumList $solidList]

    global SLF_SOLID

    return [CreateRadioButtonFrame $name $root solid Solid $SLF_SOLID $enumList]
}

proc CreateProjectionFrame { name parent } {
    set root $parent

    set projectionList [list SLF_PERSPECTIVE SLF_PARALLEL]
    set enumList [CreateSLIDEEnumList $projectionList]

    global SLF_PERSPECTIVE

    return [CreateRadioButtonFrame $name $root projection Projection $SLF_PERSPECTIVE $enumList]
}

proc CreateLightTypeFrame { name parent } {
    set root $parent

    set lightingTypeList [list SLF_AMBIENT SLF_DIRECTIONAL SLF_POINT SLF_SPOT]
    set enumList [CreateSLIDEEnumList $lightingTypeList]

    global SLF_AMBIENT

    return [CreateRadioButtonFrame $name $root type Type $SLF_AMBIENT $enumList]
}

proc CreateObjectUI { parent name } {

    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # LOD

    set lodFrame [CreateLODFrame $name $root]

    ##################################################
    # Shading

    set shadingFrame [CreateShadingFrame $name $root]

    ##################################################
    # Solid

    set solidFrame [CreateSolidFrame $name $root]

    pack $lodFrame $shadingFrame $solidFrame -side left -fill y
}

proc CreateGroupUI { parent name } {
    
    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # LOD

    set lodFrame [CreateLODFrame $name $root]

    ##################################################
    # Shading

    set shadingFrame [CreateShadingFrame $name $root]

    pack $lodFrame $shadingFrame -side left -fill y
}

proc CreateInstanceUI { parent name } {

    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # LOD

    set lodFrame [CreateLODFrame $name $root]

    ##################################################
    # Shading

    set shadingFrame [CreateShadingFrame $name $root]

    pack $lodFrame $shadingFrame -side left -fill y
}

proc CreateCameraUI { parent name } {

    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # Projection

    set projectionFrame [CreateProjectionFrame $name $root]

    ##################################################
    # Clipping

    set leftRightFrame [CreateDoubleScale $name $root leftRight left right \
	    Left Right -10 -0.5 0.5 10 0.01 1 horizontal]
    set bottomTopFrame [CreateDoubleScale $name $root bottomTop bottom top \
	    Bottom Top -10 -0.5 0.5 10 0.01 1 horizontal]
    set backFrontFrame [CreateDoubleScale $name $root backFront back front \
	    Back Front -10 -2 -0.01 -0.01 0.01 1 horizontal]

    pack $projectionFrame $leftRightFrame $bottomTopFrame $backFrontFrame -side top -fill x
}

proc CreateRGBColorUI { name parent this } {
    if { $parent == {} } {
	set root .$name
	if { [winfo exists $root] } {
	    raise $root
	    return $root
	}
	toplevel $root
    } elseif { $parent == "." } {
	set root .$name
	frame $root
    } else {
	set root $parent.$name
	toplevel $root
    }

    set varThis "[subst $name]_[subst $this]"

    set rScale [CreateScaleCmd $varThis $root r "R" 0.5 0.0 1.0 0.01 1 horizontal\
	    "CBRGBColor $root $varThis"]
    set gScale [CreateScaleCmd $varThis $root g "G" 0.5 0.0 1.0 0.01 1 horizontal\
	    "CBRGBColor $root $varThis"]
    set bScale [CreateScaleCmd $varThis $root b "B" 0.5 0.0 1.0 0.01 1 horizontal\
	    "CBRGBColor $root $varThis"]

    ###########################
    # Create the Color Sample

    set varR "[subst $varThis]_r"
    set varG "[subst $varThis]_g"
    set varB "[subst $varThis]_b"

    global $varR $varG $varB
    set byteR [RGBScaleToByte [subst $$varR]]
    set byteG [RGBScaleToByte [subst $$varG]]
    set byteB [RGBScaleToByte [subst $$varB]]

    frame $root.sample -width 200 -height 200 -bg [format \#%02x%02x%02x \
	    $byteR $byteG $byteB]

    pack $rScale $gScale $bScale $root.sample -side top -fill x

    return $root
}

proc CreateRGBColorButton { name parent this text } {
    set root $parent.$this

    set varThis "[subst $name]_[subst $this]"

    set varR "[subst $varThis]_r"
    set varG "[subst $varThis]_g"
    set varB "[subst $varThis]_b"

    global $varR $varG $varB

    set $varR 0.50
    set $varG 0.50
    set $varB 0.50

    button $root -text $text -command "CreateRGBColorUI $name $root $this"

    return $root
}

proc CreateLightUI { parent name } {

    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # Light Type

    set lightTypeFrame [CreateLightTypeFrame $name $root]

    ##################################################
    # Color

    set colorButton [CreateRGBColorButton $name $root color Color]

    ##################################################
    # Dead Distance

    set deadDistanceScale [CreateScale $name $root d0 "Dead Distance" \
	    0.1 0.01 10 0.01 1 horizontal]

    ##################################################
    # Fall Off

    set fallOffScale [CreateScale $name $root n1 "Fall Off" \
	    1 1 100 0.1 1 horizontal]

    ##################################################
    # Angular Fall Off

    set angularFallOffScale [CreateScale $name $root n2 "Angular Fall Off" \
	    1 1 100 0.1 1 horizontal]

    pack $lightTypeFrame $colorButton $deadDistanceScale $fallOffScale \
	    $angularFallOffScale -side top -fill x
}

proc CreateSurfaceUI { parent name } {

    if { $parent == {} } {
	set root ".slf_[subst $name]"
    } elseif { $parent == "." } {
	set root ".slf_[subst $name]"
    } else {
	set root "$parent.slf_[subst $name]"
    }

    toplevel $root

    ##################################################
    # Color

    set colorButton [CreateRGBColorButton $name $root color Color]

    ##################################################
    # Kamb

    set kAmbScale [CreateScale $name $root kAmb "Kamb" \
	    1.0 0.0 1.0 0.01 1 horizontal]

    ##################################################
    # Kdiff

    set kDiffScale [CreateScale $name $root kDiff "Kdiff" \
	    1.0 0.0 1.0 0.01 1 horizontal]

    ##################################################
    # Kspec

    set kSpecScale [CreateScale $name $root kSpec "Kspec" \
	    1.0 0.0 1.0 0.01 1 horizontal]

    ##################################################
    # Nphong

    set nPhongScale [CreateScale $name $root nPhong "Nphong" \
	    1 1 100 0.1 1 horizontal]

    ##################################################
    # f -- Metallic

    set metallicScale [CreateScale $name $root f "Metallic" \
	    1 1 100 0.1 1 horizontal]

    pack $colorButton $kAmbScale $kDiffScale $kSpecScale $nPhongScale \
	    $metallicScale -side top -fill x
}

# Colors
#tk_chooseColor -title "Choose a $name color" -parent $w

#CreateLightUI lite
#CreateSurfaceUI surf


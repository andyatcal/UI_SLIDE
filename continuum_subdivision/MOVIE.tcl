proc CreateMovieUI { parent name min max winx winy} {
    set subname "slf_[subst $name]"

    if { $parent == {} } {
	set root .$subname
    } elseif { $parent == "." } {
	set root .$subname
    } else {
	set root $parent.$subname
    }

    toplevel $root
    wm geometry $root +$winx+$winy

    global framesWritten delayWrite oneMore minFrame maxFrame
    set framesWritten 0
    set delayWrite 0
    set oneMore 0

    ### Make sure minFrame <= maxFrame
    if { $max < $min } {
	set maxFrame $min
	set minFrame $max
    } else {
	set maxFrame $max
	set minFrame $min
    }

    ##########################################
    # Widget
  
    frame $root.framecontrol
    frame $root.framecontrol.left
    frame $root.framecontrol.middle
    frame $root.framecontrol.right
    frame $root.cdbuttons
    frame $root.message

    ###########################################
    # Frame Control

    label $root.framecontrol.left.startlabel -text "Start"
    entry $root.framecontrol.left.startentry -textvariable start -width 5
    global start
    set start $minFrame
    pack $root.framecontrol.left.startlabel $root.framecontrol.left.startentry -side top

    set FrameSlider [CreateScaleL $name $root.framecontrol.middle frame "Current Frame" \
	    $minFrame $minFrame $maxFrame 1 1 horizontal 200 ]
    pack $root.framecontrol.middle.frame -side top

    label $root.framecontrol.right.endlabel -text "End"
    entry $root.framecontrol.right.endentry -textvariable end -width 5
    global end
    set end $maxFrame
    pack $root.framecontrol.right.endlabel $root.framecontrol.right.endentry -side top

    pack $root.framecontrol.left $root.framecontrol.middle $root.framecontrol.right \
	    -side left

    pack $root.framecontrol -side top

    ###########################################
    # CD Buttons

    set PlayButton [CreateButton $name $root.cdbuttons play "Play" "PlayVar" ]
    set WriteRibButton [CreateCButton $name $root.cdbuttons writeRib "WriteRib"]
    
    pack $root.cdbuttons.play $root.cdbuttons.writeRib -side left

    ###########################################
    # Message

    label $root.message.writeNumber -textvariable framesWritten -width 5
    label $root.message.writeLabel  -text "# Frames Written: " 
    global framesWritten
    set framesWritten 0

    pack $root.message.writeLabel $root.message.writeNumber -side left

    ###########################################
    # Widget

    pack $root.framecontrol $root.cdbuttons $root.message -side top
}

proc CalculateTickInterval { min max tickints } {
    return [expr ($max - $min)/$tickints]
}

proc CreateScaleL {name parent this text default min max resolution tickints orient length} {
    set root $parent.$this

    set varThis "[subst $name]_[subst $this]"

    global $varThis

    set $varThis $default

    set tickinterval [CalculateTickInterval $min $max $tickints]

    scale $root -label $text -from $min -to $max  \
	    -resolution $resolution -tickinterval $tickinterval \
	    -orient $orient -var $varThis -length $length

    return $root
}

proc CreateButton { name parent this text cmd } {
    set root $parent.$this

    button $root -text $text -command $cmd

    # variable associated with button
    global play
    set play 0
 
    return $root
}

proc CreateCButton { name parent this text } {
    set root $parent.$this

    checkbutton $root -text $text -variable $this

    # variable associated with button
 
    return $root
}

proc PlayVar { } {
    global play framesWritten delayWrite start end minFrame maxFrame

    ### Re-initialize variables

    set framesWritten 0
    set delayWrite 0

    if { $play == 1 } {
	set play 0
    } else {
	set play 1
    }
    
    if { $start < $minFrame } {
	set start $minFrame
    }

    if { $end > $maxFrame } {
	set end $maxFrame
    }
}

#########################################################################

proc MovieUpdate { } {
   PlayMovie
   WriteRibFile
}

proc PlayMovie { } {
    global rib_frame play start end startFrame endFrame oneMore writeRib


    #### if end or start have chars in them then it crashes 
    set endFrame [expr $end+0]
    set startFrame [expr $start+0]

    if { $play == 1 } {

	if { $rib_frame < $startFrame } {
	    set rib_frame $startFrame
	}

	if { $rib_frame < $endFrame } {
	    incr rib_frame 1
	} else {
	    set play 0

	    if { ($writeRib == 1) && ($rib_frame == $endFrame) } { 
		set oneMore 1
	    }
	}
    }
}

proc WriteRibFile { } {
    global slfwin play writeRib framesWritten delayWrite oneMore
    
    ### The write must be delayed by one so that the tcl stuff 
    ### is updated and rendered properly

    if { $delayWrite == 1 } {
	slfwin handleWriteRIB
	incr framesWritten 1
    }
    
    ### Delay the first write
    if { ($writeRib == 1) && ($play==1) } {
	set delayWrite 1
    } else {
	set delayWrite 0
    }

    ### Do an extra write after play goes to zero
    if { $oneMore == 1 } { 
	set delayWrite 1
        set oneMore 0
    }
}



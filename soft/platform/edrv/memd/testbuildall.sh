for a in flsh_*; do
    if [[ $a != flsh_fake16m ]] && [[ $a != flsh_romulator ]]; then
        if [[ ${a/_romu} == ${a} ]]; then
            mkopt="FLSH_MODEL=$a CT_ROMULATOR=no" ;
        else
            mkopt="FLSH_MODEL=${a/_romu} CT_ROMULATOR=yes" ;
        fi
        echo $mkopt
        make $mkopt clean 
        make $mkopt || exit
    fi
done

# Remove CT_ _hwptr
s/CT_\([[:alnum:]_]*\)_hwptr/hwp_\1/g
                                              
#Removes all _ and capitalizes the following letter
#FIXME well ... 
:again_1
s/\(hwp_[[:alnum:]_]*\)_[aA]\([[:alnum:]_]*\)/\1A\2/g
t again_1                                  
                                           
:again_2                                   
s/\(hwp_[[:alnum:]_]*\)_[bB]\([[:alnum:]_]*\)/\1B\2/g
t again_2                                         
                                                  
:again_3                                          
s/\(hwp_[[:alnum:]_]*\)_[cC]\([[:alnum:]_]*\)/\1C\2/g
t again_3                                         
                                                  
:again_4                                          
s/\(hwp_[[:alnum:]_]*\)_[dD]\([[:alnum:]_]*\)/\1D\2/g
t again_4                                  
                                           
:again_5                                   
s/\(hwp_[[:alnum:]_]*\)_[eE]\([[:alnum:]_]*\)/\1E\2/g
t again_5                                  
                                           
:again_6                                   
s/\(hwp_[[:alnum:]_]*\)_[fF]\([[:alnum:]_]*\)/\1F\2/g
t again_6                                  
                                           
:again_7                                   
s/\(hwp_[[:alnum:]_]*\)_[gG]\([[:alnum:]_]*\)/\1G\2/g
t again_7                                  
                                           
:again_8                                   
s/\(hwp_[[:alnum:]_]*\)_[hH]\([[:alnum:]_]*\)/\1H\2/g
t again_8                                  
                                           
:again_9                                   
s/\(hwp_[[:alnum:]_]*\)_[iI]\([[:alnum:]_]*\)/\1I\2/g
t again_9                                  
                                           
:again_10                                  
s/\(hwp_[[:alnum:]_]*\)_[jJ]\([[:alnum:]_]*\)/\1J\2/g
t again_10                                 
                                           
:again_11                                  
s/\(hwp_[[:alnum:]_]*\)_[kK]\([[:alnum:]_]*\)/\1K\2/g
t again_11                                 
                                           
:again_12                                  
s/\(hwp_[[:alnum:]_]*\)_[lL]\([[:alnum:]_]*\)/\1L\2/g
t again_12                                 
                                           
:again_13                                  
s/\(hwp_[[:alnum:]_]*\)_[mM]\([[:alnum:]_]*\)/\1M\2/g
t again_13                                 
                                           
:again_14                                  
s/\(hwp_[[:alnum:]_]*\)_[nN]\([[:alnum:]_]*\)/\1N\2/g
t again_14                                 
                                           
:again_15                                  
s/\(hwp_[[:alnum:]_]*\)_[oO]\([[:alnum:]_]*\)/\1O\2/g
t again_15                                 
                                           
:again_16                                  
s/\(hwp_[[:alnum:]_]*\)_[pP]\([[:alnum:]_]*\)/\1P\2/g
t again_16                                 
                                           
:again_17                                  
s/\(hwp_[[:alnum:]_]*\)_[qQ]\([[:alnum:]_]*\)/\1Q\2/g
t again_17                                 
                                           
:again_18                                  
s/\(hwp_[[:alnum:]_]*\)_[rR]\([[:alnum:]_]*\)/\1R\2/g
t again_18                                 
                                           
:again_19                                  
s/\(hwp_[[:alnum:]_]*\)_[sS]\([[:alnum:]_]*\)/\1S\2/g
t again_19                                        
                                                  
# t is special because types are to finish with _ t
:again_20                                         
s/\(hwp_[[:alnum:]_]*\)_[tT]\([[:alnum:]_]\*\)/\1T\2/g
t again_20                                  
                                            
:again_21                                   
s/\(hwp_[[:alnum:]_]*\)_[uU]\([[:alnum:]_]*\)/\1U\2/g
t again_21                                 
                                           
:again_22                                  
s/\(hwp_[[:alnum:]_]*\)_[vV]\([[:alnum:]_]*\)/\1V\2/g
t again_22                                 
                                           
:again_23                                  
s/\(hwp_[[:alnum:]_]*\)_[wW]\([[:alnum:]_]*\)/\1W\2/g
t again_23                                 
                                           
:again_24                                  
s/\(hwp_[[:alnum:]_]*\)_[xX]\([[:alnum:]_]*\)/\1X\2/g
t again_24                                 
                                           
:again_25                                  
s/\(hwp_[[:alnum:]_]*\)_[yY]\([[:alnum:]_]*\)/\1Y\2/g
t again_25                                 
                                           
:again_26                                  
s/\(hwp_[[:alnum:]_]*\)_[zZ]\([[:alnum:]_]*\)/\1Z\2/g
t again_26






function! Modem_Get_Var_size()
    normal gg
    let line_cur = 1
    let totle_line = line('$')

    "check sencond item is addr or size
    if stridx(split(getline(line_cur), '\s\+')[1], "0x") >= 0
        let num_is_size = 1
    else
        let num_is_size = 0
    endif

    while line_cur <= totle_line
        if (num_is_size == 1)
            let tag_size = str2nr(split(getline(line_cur), '\s\+')[1], 16)
            let tag_size_k=tag_size/1024
        else
            if line_cur == totle_line
                exe '$d'
                break
            endif

            let base = str2nr(split(getline(line_cur), '\s\+')[1], 16)
            let next_base = str2nr(split(getline(line_cur+1), '\s\+')[1], 16)
            let tag_size=(next_base - base)
            let tag_size_k=tag_size/1024
        endif
        exec line_cur . 's/^/   /'
        exec line_cur .'s/^/\=tag_size/'
        exec line_cur . 's/^/   /'
        exec line_cur . 's/^/\=tag_size_k/'
        exec line_cur .'s/ /k/'

        let line_cur += 1

    endwhile
    exe "w!"
endfunction

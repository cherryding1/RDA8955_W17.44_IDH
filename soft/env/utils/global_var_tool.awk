BEGIN{print_part = 0;read_next = 0; in_order_step = 0;}
{
    if (var_in_order == 0)
    {
        if (/^\s*\.(data|bss)\./) 
        {
            if ($2 == "") {
                backup=$0;
                read_next = 1;
                next;
            } 
            else 
            {
                read_next = 0;
                printf("%s   %s\n", $1, $3);
            }
        }
        else 
        {
            if (read_next == 1) 
            {
                printf("%s   %s\n", backup,$2);
                read_next = 0;
            }
        }

        if (/Common symbol/)
        {
            print_part = 1;
            next;
        }
        if (/Discarded input/)
        {
            print_part = 0;
            next;
        }
        if (print_part == 1)
        {
            if ($1 == "")
                next;

            if($2 == "") 
            {
                save_line = $0
                save_flag=1;
            }
            else if (/^ /)
            {
                if (save_flag == 1)
                {
                    printf( "%s   %s\n", save_line, $1);
                }
            }
            else
                printf("%s %s\n", $1,$2);
        }
    }
    else
    {
        if ($0 ~ /^\.bss/)
            in_order_step=1;
        if (in_order_step == 0) 
            next;
        if ((in_order_step == 1) && ($0 ~ /^\s+\.bss/)) 
        {
            in_order_step=2;
            next;
        }
        if (in_order_step == 2)
        {
            if ($1 != "")
            {
                printf("%s    %s\n", $2, gensub("0xffffffff82", "",1,$1));
            }
            else
                exit
        }
    }
}

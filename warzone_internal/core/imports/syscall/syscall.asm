public _do_syscall

.code
_do_syscall proc

	mov		r10, rcx
	mov		rax, [rsp + 40]

	add		rsp, 16
	syscall
	sub		rsp, 16

	ret

_do_syscall endp

end

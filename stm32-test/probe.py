from psh.tools.basic import run_psh, assert_only_prompt, assert_prompt

def harness(p):
	p.send("\n")
	p.sendline('ls')
	p.expect_exact("syspage")
	p.send("\n")
	#assert_prompt(p)

#include <r_arch.h>
#include "minunit.h"

bool test_arch(void) {
#if 0
	RArch *a = r_arch_new ();
	mu_assert_notnull (a, "r_arch_new");
	RArchSetup setup = {
		.arch = "x86",
		.endian = R_SYS_ENDIAN_LITTLE,
		.bits = 32,
	};
	RArchInstance *ai = r_arch_instance (a, &setup);
	RArchInstruction ins = {
		
	};
	r_arch_instance_encode (ai, 
#endif
	mu_end;
}

static RArchPlugin test_plugin = {
	.name = "test_plugin",
	.arch = "test",
	.author = "radare2",
	.desc = "Example RArch plugin",
};

static bool test_arch_register_plugin(void) {
	RArch *a = r_arch_new ();
	bool res = r_arch_add (a, &test_plugin);
	mu_assert ("test_plugin should be registered the first time", res);
	res = r_arch_add (a, &test_plugin);
	mu_assert ("test_plugin was already registered", !res);

	RArchPlugin *ap = r_arch_get_plugin (a, "test_plugin");
	mu_assert_notnull (ap, "test_plugin should be found");
	mu_assert_ptreq (ap, &test_plugin, "test_plugin and ap should be the same");

	res = r_arch_del (a, ap);
	mu_assert ("test_plugin should be deleted", res);
	res = r_arch_del (a, ap);
	mu_assert ("test_plugin was already deleted", !res);

	ap = r_arch_get_plugin (a, "test_plugin");
	mu_assert_null (ap, "test_plugin should NOT be found, because deleted");

	r_arch_free (a);
	mu_end;
}

static bool test_arch_bf(void) {
	RArchInstruction ins;
	bool res;

	RArch *a = r_arch_new ();
	mu_assert_notnull (a, "r_arch_new");

	RArchPlugin *ap = r_arch_get_plugin (a, "bf");
	mu_assert_notnull (a, "r_arch_get_plugin(bf)");
	RArchSession *as = r_arch_session_new (a, ap, NULL);
	mu_assert_notnull (as, "r_arch_session_new()");

	r_arch_instruction_init (&ins);
	r_strbuf_setf (&ins.code, "nop");
	res = r_arch_session_encode (as, &ins, R_ARCH_OPTION_CODE);
	mu_assert ("r_arch_session_encode", res);
	mu_assert_eq (ins.size, 1, "encoded instruction size");
	mu_assert_eq (r_strbuf_get (&ins.data)[0], (char)0x90, "encoded instruction size");

	r_strbuf_setbin (&ins.data, (const ut8*)"\x2b", 1);
	res = r_arch_session_decode (as, &ins, R_ARCH_OPTION_CODE);
	mu_assert ("r_arch_session_decode", res);

	const char *opstr = r_strbuf_get (&ins.code);
	mu_assert_streq (opstr, "inc [ptr]", "invalid decoded instruction");

	r_arch_instruction_fini (&ins);

	r_arch_session_free (as);
	r_arch_free (a);

	mu_end;
}

int all_tests() {
	mu_run_test (test_arch_bf);
	mu_run_test (test_arch);
	mu_run_test (test_arch_register_plugin);
	return tests_passed != tests_run;
}

int main(int argc, char **argv) {
	return all_tests();
}
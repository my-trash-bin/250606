# ABT - All Branch Test

Test all branches

For example,

```c
err_t init_two_my_resources(res_t **out_a, res_t **out_b) {
    res_t *const a = init_my_resource();
    if (!a) {
        return true;
    }
    res_t *const b = init_my_resource();
    if (!b) {
        return true;
    }
    *out_a = a;
    *out_b = b;
    return false;
}
```

In this function, there is a critical bug that forget to destroy `a` on second `init_my_resource()` failure.

You can test this function with the following mock:

```c
// allocated my_resource count
static size_t counter = 0;

err_t init_my_resource() {
    static res_t *(*real)(void) = NULL;

	if (!real)
		real = (res_t *(*)(void))dlsym(RTLD_NEXT, "init_my_resource");

    // return real result if not testing
    if (!abt_started()) {
        return real();
    }

    // we will test two cases
    size_t branch = abt_branch(2);

    switch (branch) {
    case 0:
        // case 0: simulate allocation failure
        abt_mark_as_failure();
        return NULL;
    case 1:
        // case 1: simulate allocation success
        res_t *const result = real();
        if (!result) {
            // allocation failure during test == test error
            abt_error("init_my_resource() failed");
        }
        counter++;
        return result;
    }
}

void destroy_my_resource(res_t *resource) {
    static void (*real)(res_t *resource) = NULL;

	if (!real)
		real = (void (*)(res_t *resource))dlsym(RTLD_NEXT, "destroy_my_resource");

    if (!abt_started()) {
        real(resource);
        return;
    }

    if (resource) {
        counter--;
    }
    real(resource);
}

void my_resource_mock_finalize(void) {
    if (counter) {
        fprintf(stderr, "leaked my_resource count: %zu", counter);
        abt_fail("You've leaked my_resource.");
    }
}
```

Usage is so simple:

```c
int main(void) {
    abt_start();
    res_t *a;
    res_t *b;
    const err_t result = init_two_my_resources(&a, &b);
    if (result && !abt_is_successful_branch()) {
        abt_fail("At least one allocation failed but result is OK");
    }
    if (!result && abt_is_successful_branch()) {
        abt_fail("All allocation succeed but result is not OK");
    }
    if (!result) {
        destroy_my_resource(a);
        destroy_my_resource(b);
    }
    my_resource_mock_finalize();
    abt_end();
}
```

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

static inline int arch_atomic_swap(volatile int * x, int v) {
    __asm__ __volatile__("xchg %0, %1" : "=r"(v), "=m"(*x) : "0"(v) : "memory");
    return v;
}

static inline void arch_atomic_store(volatile int * p, int x) {
    __asm__ __volatile__("movl %1, %0" : "=m"(*p) : "r"(x) : "memory");
}

static inline void arch_atomic_inc(volatile int * x) {
    __asm__ __volatile__("lock; incl %0" : "=m"(*x) : "m"(*x) : "memory");
}

static inline void arch_atomic_dec(volatile int * x) {
    __asm__ __volatile__("lock; decl %0" : "=m"(*x) : "m"(*x) : "memory");
}

void spin_wait(volatile int * addr, volatile int * waiters) {
    if (waiters) {
        arch_atomic_inc(waiters);
    }
    while (*addr) {
        switch_task(1);
    }
    if (waiters) {
        arch_atomic_dec(waiters);
    }
}

void spin_lock(spin_lock_t lock) {
    while (arch_atomic_swap(lock, 1)) {
        spin_wait(lock, lock+1);
    }
}

void spin_init(spin_lock_t lock) {
    lock[0] = 0;
    lock[1] = 0;
}

void spin_unlock(spin_lock_t lock) {
    if (lock[0]) {
        arch_atomic_store(lock, 0);
        if (lock[1])
            switch_task(1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

 16 #define ATOMIC_INIT(i)  { (i) }
 17 
 18 /**
 19  * atomic_read - read atomic variable
 20  * @v: pointer of type atomic_t
 21  *
 22  * Atomically reads the value of @v.
 23  */
 24 static __always_inline int atomic_read(const atomic_t *v)
 25 {
 26         return READ_ONCE((v)->counter);
 27 }
 28 
 29 /**
 30  * atomic_set - set atomic variable
 31  * @v: pointer of type atomic_t
 32  * @i: required value
 33  *
 34  * Atomically sets the value of @v to @i.
 35  */
 36 static __always_inline void atomic_set(atomic_t *v, int i)
 37 {
 38         WRITE_ONCE(v->counter, i);
 39 }
 40 
 41 /**
 42  * atomic_add - add integer to atomic variable
 43  * @i: integer value to add
 44  * @v: pointer of type atomic_t
 45  *
 46  * Atomically adds @i to @v.
 47  */
 48 static __always_inline void atomic_add(int i, atomic_t *v)
 49 {
 50         asm volatile(LOCK_PREFIX "addl %1,%0"
 51                      : "+m" (v->counter)
 52                      : "ir" (i));
 53 }
 54 
 55 /**
 56  * atomic_sub - subtract integer from atomic variable
 57  * @i: integer value to subtract
 58  * @v: pointer of type atomic_t
 59  *
 60  * Atomically subtracts @i from @v.
 61  */
 62 static __always_inline void atomic_sub(int i, atomic_t *v)
 63 {
 64         asm volatile(LOCK_PREFIX "subl %1,%0"
 65                      : "+m" (v->counter)
 66                      : "ir" (i));
 67 }
 68 
 69 /**
 70  * atomic_sub_and_test - subtract value from variable and test result
 71  * @i: integer value to subtract
 72  * @v: pointer of type atomic_t
 73  *
 74  * Atomically subtracts @i from @v and returns
 75  * true if the result is zero, or false for all
 76  * other cases.
 77  */
 78 static __always_inline int atomic_sub_and_test(int i, atomic_t *v)
 79 {
 80         GEN_BINARY_RMWcc(LOCK_PREFIX "subl", v->counter, "er", i, "%0", "e");
 81 }
 82 
 83 /**
 84  * atomic_inc - increment atomic variable
 85  * @v: pointer of type atomic_t
 86  *
 87  * Atomically increments @v by 1.
 88  */
 89 static __always_inline void atomic_inc(atomic_t *v)
 90 {
 91         asm volatile(LOCK_PREFIX "incl %0"
 92                      : "+m" (v->counter));
 93 }
 94 
 95 /**
 96  * atomic_dec - decrement atomic variable
 97  * @v: pointer of type atomic_t
 98  *
 99  * Atomically decrements @v by 1.
100  */
101 static __always_inline void atomic_dec(atomic_t *v)
102 {
103         asm volatile(LOCK_PREFIX "decl %0"
104                      : "+m" (v->counter));
105 }
106 
107 /**
108  * atomic_dec_and_test - decrement and test
109  * @v: pointer of type atomic_t
110  *
111  * Atomically decrements @v by 1 and
112  * returns true if the result is 0, or false for all other
113  * cases.
114  */
115 static __always_inline int atomic_dec_and_test(atomic_t *v)
116 {
117         GEN_UNARY_RMWcc(LOCK_PREFIX "decl", v->counter, "%0", "e");
118 }
119 
120 /**
121  * atomic_inc_and_test - increment and test
122  * @v: pointer of type atomic_t
123  *
124  * Atomically increments @v by 1
125  * and returns true if the result is zero, or false for all
126  * other cases.
127  */
128 static __always_inline int atomic_inc_and_test(atomic_t *v)
129 {
130         GEN_UNARY_RMWcc(LOCK_PREFIX "incl", v->counter, "%0", "e");
131 }
132 
133 /**
134  * atomic_add_negative - add and test if negative
135  * @i: integer value to add
136  * @v: pointer of type atomic_t
137  *
138  * Atomically adds @i to @v and returns true
139  * if the result is negative, or false when
140  * result is greater than or equal to zero.
141  */
142 static __always_inline int atomic_add_negative(int i, atomic_t *v)
143 {
144         GEN_BINARY_RMWcc(LOCK_PREFIX "addl", v->counter, "er", i, "%0", "s");
145 }
146 
147 /**
148  * atomic_add_return - add integer and return
149  * @i: integer value to add
150  * @v: pointer of type atomic_t
151  *
152  * Atomically adds @i to @v and returns @i + @v
153  */
154 static __always_inline int atomic_add_return(int i, atomic_t *v)
155 {
156         return i + xadd(&v->counter, i);
157 }
158 
159 /**
160  * atomic_sub_return - subtract integer and return
161  * @v: pointer of type atomic_t
162  * @i: integer value to subtract
163  *
164  * Atomically subtracts @i from @v and returns @v - @i
165  */
166 static __always_inline int atomic_sub_return(int i, atomic_t *v)
167 {
168         return atomic_add_return(-i, v);
169 }
170 
171 #define atomic_inc_return(v)  (atomic_add_return(1, v))
172 #define atomic_dec_return(v)  (atomic_sub_return(1, v))
173 
174 static __always_inline int atomic_cmpxchg(atomic_t *v, int old, int new)
175 {
176         return cmpxchg(&v->counter, old, new);
177 }
178 
179 static inline int atomic_xchg(atomic_t *v, int new)
180 {
181         return xchg(&v->counter, new);
182 }
183 
184 #define ATOMIC_OP(op)                                                   \
185 static inline void atomic_##op(int i, atomic_t *v)                      \
186 {                                                                       \
187         asm volatile(LOCK_PREFIX #op"l %1,%0"                           \
188                         : "+m" (v->counter)                             \
189                         : "ir" (i)                                      \
190                         : "memory");                                    \
191 }
192 
193 ATOMIC_OP(and)
194 ATOMIC_OP(or)
195 ATOMIC_OP(xor)
196 
197 #undef ATOMIC_OP
198 
199 /**
200  * __atomic_add_unless - add unless the number is already a given value
201  * @v: pointer of type atomic_t
202  * @a: the amount to add to v...
203  * @u: ...unless v is equal to u.
204  *
205  * Atomically adds @a to @v, so long as @v was not already @u.
206  * Returns the old value of @v.
207  */
208 static __always_inline int __atomic_add_unless(atomic_t *v, int a, int u)
209 {
210         int c, old;
211         c = atomic_read(v);
212         for (;;) {
213                 if (unlikely(c == (u)))
214                         break;
215                 old = atomic_cmpxchg((v), c, c + (a));
216                 if (likely(old == c))
217                         break;
218                 c = old;
219         }
220         return c;
221 }
222 
223 /**
224  * atomic_inc_short - increment of a short integer
225  * @v: pointer to type int
226  *
227  * Atomically adds 1 to @v
228  * Returns the new value of @u
229  */
230 static __always_inline short int atomic_inc_short(short int *v)
231 {
232         asm(LOCK_PREFIX "addw $1, %0" : "+m" (*v));
233         return *v;
234 }
235 
236 #ifdef CONFIG_X86_32
237 # include <asm/atomic64_32.h>
238 #else
239 # include <asm/atomic64_64.h>
240 #endif


 12 /**
 13  * atomic64_read - read atomic64 variable
 14  * @v: pointer of type atomic64_t
 15  *
 16  * Atomically reads the value of @v.
 17  * Doesn't imply a read memory barrier.
 18  */
 19 static inline long atomic64_read(const atomic64_t *v)
 20 {
 21         return READ_ONCE((v)->counter);
 22 }
 23 
 24 /**
 25  * atomic64_set - set atomic64 variable
 26  * @v: pointer to type atomic64_t
 27  * @i: required value
 28  *
 29  * Atomically sets the value of @v to @i.
 30  */
 31 static inline void atomic64_set(atomic64_t *v, long i)
 32 {
 33         WRITE_ONCE(v->counter, i);
 34 }
 35 
 36 /**
 37  * atomic64_add - add integer to atomic64 variable
 38  * @i: integer value to add
 39  * @v: pointer to type atomic64_t
 40  *
 41  * Atomically adds @i to @v.
 42  */
 43 static __always_inline void atomic64_add(long i, atomic64_t *v)
 44 {
 45         asm volatile(LOCK_PREFIX "addq %1,%0"
 46                      : "=m" (v->counter)
 47                      : "er" (i), "m" (v->counter));
 48 }
 49 
 50 /**
 51  * atomic64_sub - subtract the atomic64 variable
 52  * @i: integer value to subtract
 53  * @v: pointer to type atomic64_t
 54  *
 55  * Atomically subtracts @i from @v.
 56  */
 57 static inline void atomic64_sub(long i, atomic64_t *v)
 58 {
 59         asm volatile(LOCK_PREFIX "subq %1,%0"
 60                      : "=m" (v->counter)
 61                      : "er" (i), "m" (v->counter));
 62 }
 63 
 64 /**
 65  * atomic64_sub_and_test - subtract value from variable and test result
 66  * @i: integer value to subtract
 67  * @v: pointer to type atomic64_t
 68  *
 69  * Atomically subtracts @i from @v and returns
 70  * true if the result is zero, or false for all
 71  * other cases.
 72  */
 73 static inline int atomic64_sub_and_test(long i, atomic64_t *v)
 74 {
 75         GEN_BINARY_RMWcc(LOCK_PREFIX "subq", v->counter, "er", i, "%0", "e");
 76 }
 77 
 78 /**
 79  * atomic64_inc - increment atomic64 variable
 80  * @v: pointer to type atomic64_t
 81  *
 82  * Atomically increments @v by 1.
 83  */
 84 static __always_inline void atomic64_inc(atomic64_t *v)
 85 {
 86         asm volatile(LOCK_PREFIX "incq %0"
 87                      : "=m" (v->counter)
 88                      : "m" (v->counter));
 89 }
 90 
 91 /**
 92  * atomic64_dec - decrement atomic64 variable
 93  * @v: pointer to type atomic64_t
 94  *
 95  * Atomically decrements @v by 1.
 96  */
 97 static __always_inline void atomic64_dec(atomic64_t *v)
 98 {
 99         asm volatile(LOCK_PREFIX "decq %0"
100                      : "=m" (v->counter)
101                      : "m" (v->counter));
102 }
103 
104 /**
105  * atomic64_dec_and_test - decrement and test
106  * @v: pointer to type atomic64_t
107  *
108  * Atomically decrements @v by 1 and
109  * returns true if the result is 0, or false for all other
110  * cases.
111  */
112 static inline int atomic64_dec_and_test(atomic64_t *v)
113 {
114         GEN_UNARY_RMWcc(LOCK_PREFIX "decq", v->counter, "%0", "e");
115 }
116 
117 /**
118  * atomic64_inc_and_test - increment and test
119  * @v: pointer to type atomic64_t
120  *
121  * Atomically increments @v by 1
122  * and returns true if the result is zero, or false for all
123  * other cases.
124  */
125 static inline int atomic64_inc_and_test(atomic64_t *v)
126 {
127         GEN_UNARY_RMWcc(LOCK_PREFIX "incq", v->counter, "%0", "e");
128 }
129 
130 /**
131  * atomic64_add_negative - add and test if negative
132  * @i: integer value to add
133  * @v: pointer to type atomic64_t
134  *
135  * Atomically adds @i to @v and returns true
136  * if the result is negative, or false when
137  * result is greater than or equal to zero.
138  */
139 static inline int atomic64_add_negative(long i, atomic64_t *v)
140 {
141         GEN_BINARY_RMWcc(LOCK_PREFIX "addq", v->counter, "er", i, "%0", "s");
142 }
143 
144 /**
145  * atomic64_add_return - add and return
146  * @i: integer value to add
147  * @v: pointer to type atomic64_t
148  *
149  * Atomically adds @i to @v and returns @i + @v
150  */
151 static __always_inline long atomic64_add_return(long i, atomic64_t *v)
152 {
153         return i + xadd(&v->counter, i);
154 }
155 
156 static inline long atomic64_sub_return(long i, atomic64_t *v)
157 {
158         return atomic64_add_return(-i, v);
159 }
160 
161 #define atomic64_inc_return(v)  (atomic64_add_return(1, (v)))
162 #define atomic64_dec_return(v)  (atomic64_sub_return(1, (v)))
163 
164 static inline long atomic64_cmpxchg(atomic64_t *v, long old, long new)
165 {
166         return cmpxchg(&v->counter, old, new);
167 }
168 
169 static inline long atomic64_xchg(atomic64_t *v, long new)
170 {
171         return xchg(&v->counter, new);
172 }
173 
174 /**
175  * atomic64_add_unless - add unless the number is a given value
176  * @v: pointer of type atomic64_t
177  * @a: the amount to add to v...
178  * @u: ...unless v is equal to u.
179  *
180  * Atomically adds @a to @v, so long as it was not @u.
181  * Returns the old value of @v.
182  */
183 static inline int atomic64_add_unless(atomic64_t *v, long a, long u)
184 {
185         long c, old;
186         c = atomic64_read(v);
187         for (;;) {
188                 if (unlikely(c == (u)))
189                         break;
190                 old = atomic64_cmpxchg((v), c, c + (a));
191                 if (likely(old == c))
192                         break;
193                 c = old;
194         }
195         return c != (u);
196 }
197 
198 #define atomic64_inc_not_zero(v) atomic64_add_unless((v), 1, 0)
199 
200 /*
201  * atomic64_dec_if_positive - decrement by 1 if old value positive
202  * @v: pointer of type atomic_t
203  *
204  * The function returns the old value of *v minus 1, even if
205  * the atomic variable, v, was not decremented.
206  */
207 static inline long atomic64_dec_if_positive(atomic64_t *v)
208 {
209         long c, old, dec;
210         c = atomic64_read(v);
211         for (;;) {
212                 dec = c - 1;
213                 if (unlikely(dec < 0))
214                         break;
215                 old = atomic64_cmpxchg((v), c, dec);
216                 if (likely(old == c))
217                         break;
218                 c = old;
219         }
220         return dec;
221 }
222 
223 #define ATOMIC64_OP(op)                                                 \
224 static inline void atomic64_##op(long i, atomic64_t *v)                 \
225 {                                                                       \
226         asm volatile(LOCK_PREFIX #op"q %1,%0"                           \
227                         : "+m" (v->counter)                             \
228                         : "er" (i)                                      \
229                         : "memory");                                    \
230 }

#endif
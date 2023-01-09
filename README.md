# TexasHoldemProbabilityCalculator
Command Line calculator for winning probability of hole cards using monte carlo method

***This is for purely entertainment purposes and is not intended for professional use**

Hand evaluation algorithm adapted and implemented from https://jonathanhsiao.com/blog/evaluating-poker-hands-with-bit-math
.

The article outlines the algorithm but does not offer code samples so this repository can be used as reference. The relevant functions are generateScore() and tieBreaker()

The mathematics in the article were not proved so you can find my proofs for each theorem below (IN PROGRESS):

Let the following binary bit field represent a 5 card hand:

$0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000$

With each 4 bit grouping representing the four cards of each rank.

Example: an ace 4 of a kind with a king can be represented as  $1111\ 0001\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000\ 0000$

The last 8 bits are not representative of any cards and exists to make calculations simpler.

So, a 4 of a kind hand can be represented using the bit field as:
$2^{4n+3} + 2^{4n+2} + 2^{4n+1} + 2^{4n} + 2^{4k}$

Where $k,\ n \in \mathbb{N}\ 2 < k,\ n < 15$

Theorem 1: Every 4 of a kind bit representation %(modulo) 15 = 1

Proof:

We wish to prove $(2^{4n+3} + 2^{4n+2} + 2^{4n+1} + 2^{4n} + 2^{4k})\ \\% \ 15 = 1$

Taking the LHS:

$LHS = (2^{4n+3} + 2^{4n+2} + 2^{4n+1} + 2^{4n} + 2^{4k})\ \\% \ 15\$

$= (2^{4n}(2^3 + 2^2 + 2 + 1) + 2^{4k})\  \\% \  15$

$= (((2^{4n}\  \\% \ 15)((2^3\ \\%\ 15 + 2^2\ \\%\ 15+ 2\ \\%\ 15+ 1\ \\%\ 15)\ \\%\ 15) \\%\ 15) + 2^{4k} \ \\%\ 15)\  \\% \  15$

Notice

$2^3\ \\%\ 15 = 8$

$2^2\ \\%\ 15 = 4$

$2\ \\%\ 15 = 2$

$1\ \\%\ 15 = 1$

$2^3\ \\%\ 15 + 2^2\ \\%\ 15+ 2\ \\%\ 15+ 1 \\%\ = 15$

$15\ \\%\ 15 = 0$

So $LHS = (2^{4k}\ \\%\ 15)\ \\%\ 15 = 2^{4k}\ \\%\ 15$

To solve $2^{4k}\ \\%\ 15$, we make use of the following lemma:

$2^{m+4} \equiv 2^m\ (mod\ 15)$ for every $m \in \mathbb{Z} > 0$

Proof:

let $m \in \mathbb{Z}$

$2^{m+4} - 2^m = 2^m*2^4 - 2^m = 2^m(2^4-1) = 15 * 2^m = 15l,\ l \in \mathbb{Z} > 0$

By the definition of congruence, $2^{m+4} \equiv 2^m\ (mod\ 15)\ \blacksquare$

So using the proven lemma, we know $2^4k \equiv 2^0\ (mod\ 15) $

And since $2^0\ \\%\ 15 = 1$, we can conclude $2^4k\ \\%\ 15 = 1$

Therefore, $LHS = 1$ as desired $\blacksquare$



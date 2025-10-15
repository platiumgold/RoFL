# lab1

исходная SRS:

$$
\tau = \left\{
\begin{array}{}
(1) & babab & \to & aabab & \quad & (8) & baab & \to & baa \\
(2) & bbaab & \to & bbbab & \quad & (9) & bbabbaa & \to & bbaaa \\
(3) & bbb & \to & bb & \quad & (10) & aabbabba & \to & bbaa \\
(4) & abaaab & \to & baba & \quad & (11) & aabaababab & \to & bbb \\
(5) & aaa & \to & \varepsilon & \quad & (12) & baababbaa & \to & bbabbabba \\
(6) & bbaaaab & \to & \varepsilon \\
(7) & ababbab & \to & ababaab \\
\end{array}
\right.
$$

## завершимость

в системе найден цикл

$ \boxed{\text{bbaab}}\text{ab} \xrightarrow{(2)} \text{bb}\boxed{\text{babab}} \xrightarrow{(1)} \text{bbaabab}$
=> система незавершима

## Классы эквивалентности

Рассмотрим симметричную SRS:

$$
\tau_{sym} = \left\{
\begin{array}{}
(1) & babab & \leftrightarrow & aabab & \quad & (8) & baab & \leftrightarrow & baa \\
(2) & bbaab & \leftrightarrow & bbbab & \quad & (9) & bbabbaa & \leftrightarrow & bbaaa \\
(3) & bbb & \leftrightarrow & bb & \quad & (10) & aabbabba & \leftrightarrow & bbaa \\
(4) & abaaab & \leftrightarrow & baba & \quad & (11) & aabaababab & \leftrightarrow & bbb \\
(5) & aaa & \leftrightarrow & \varepsilon & \quad & (12) & baababbaa & \leftrightarrow & bbabbabba \\
(6) & bbaaaab & \leftrightarrow & \varepsilon \\
(7) & ababbab & \leftrightarrow & ababaab \\
\end{array}
\right.
$$

$$
\boxed{\epsilon} \xrightarrow{(6) \xleftarrow{}} \boxed{bb}aaaab \xrightarrow{(3) \xleftarrow{}} \boxed{bbb}aaaab 
\xrightarrow{(11) \xleftarrow{}} aa\boxed{baab}abbaaaab \xrightarrow{(8) \xrightarrow{}} aab\boxed{aaa}bbaaaab \xrightarrow{(5) \xrightarrow{}} 
aa\boxed{bbb}aaaab \xrightarrow{(3) \xrightarrow{}} aa\boxed{bbaaaab} \xrightarrow{(6) \xrightarrow{}} aa
$$

таким образом, если дважды применить эту цепочку, то получим
$\epsilon \xrightarrow{} ... \xrightarrow{} aa \xrightarrow{} ... \xrightarrow{} aaaa \xrightarrow{} a$

$=> a \equiv \epsilon$

рассмотрим еще одну цепочку
$\epsilon \xrightarrow{} \boxed{bb}aaaab \xrightarrow{} b\boxed{bbaaaab} \xrightarrow{} b$

$=> b \equiv \epsilon$

итого: $a \equiv b \equiv \epsilon$

а это означает, что у нас всего один класс эквивалентности и любое слово сводится к пустому слову

## конфлюэнтность

$\boxed{abaaab} \xrightarrow{(4)} baba$

$ab\boxed{aaa}b \xrightarrow{(5)} abb$
=> система неконфлюэнтна

## построение эквивалентной SRS

используя установленный факт
$a \equiv b \equiv \epsilon$
и взяв порядок правил по убыванию длины слова, получаем новую систему

$$
\tau' = \left\{
\begin{array}{}
(1) & a & \to & \epsilon\\
(2) & b & \to & \epsilon\\
\end{array}
\right.
$$

она очевидно минимальна и конфлюэнтна и описывает тот же класс эквивалентности, что и исходная система

## тестировать нечего(

так как у системы 1 всеобъемлющий класс эквивалентности, то у нее несуществует нетривиальных инвариантов, необходимых
для метатестирования, потому что в противном случае, если бы таковой нашелся, то нашлось бы и разбиение на несколько классов.
Фазз-тестирование также не имеет смысла, так как любое слово сводится к пустому слову.

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

## классы эквивалентности

рассмотрим правила системы с введенным отношением ↔, разбивающим все термы на классы эквивалентности:

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

$=> a ↔ \epsilon$

рассмотрим еще одну цепочку
$\epsilon \xrightarrow{} \boxed{bb}aaaab \xrightarrow{} b\boxed{bbaaaab} \xrightarrow{} b$

$=> b ↔ \epsilon$

итого: $a ↔ b ↔ \epsilon$

а это означает, что у нас всего один класс эквивалентности и любое слово сводится к пустому слову

## конфлюэнтность

$\boxed{abaaab} \xrightarrow{(4)} baba$

$ab\boxed{aaa}b \xrightarrow{(5)} abb$
=> система неконфлюэнтна

## построение эквивалентной SRS

используя установленный факт
$a ↔ b ↔ \epsilon$
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
для метатестирования, потому что в противном случае, если бы таковые нашлись, то нашлось бы и разбиение на несколько классов.
Фазз-тестирование также не имеет смысла, так как любое слово сводится к пустому слову.

## еще одна система

было принято решение убрать правила (6) и (5), чтобы $\epsilon$ находилось в отдельном классе.

$$
\tau_{1} = \left\{
\begin{array}{}
(1) & babab & \to & aabab & \quad & (6) & baab & \to & baa \\
(2) & bbaab & \to & bbbab & \quad & (7) & bbabbaa & \to & bbaaa \\
(3) & bbb & \to & bb & \quad & (8) & aabbabba & \to & bbaa \\
(4) & abaaab & \to & baba & \quad & (9) & aabaababab & \to & bbb \\
(5) & ababbab & \to & ababaab & \quad & (10) & baababbaa & \to & bbabbabba \\
\end{array}
\right.
$$

количество классов эквивалентности бесконечно, т.к.
{a, aa, aaa ...} не эквивалентны друг другу

построим $\tau'_{1}$ таким образом:

введем порядок shortlex и развернем правила (2) и (10), чтобы система стала завершимой

к этой уже завершимой системе применим алгоритм Кнута-Бендикса и получим нашу предположительно эквивалентную систему:

$$
\tau'_{1} = \left\{
\begin{array}{}
(1) & bbb & \to & bb & \quad & (4) & abb & \to & bb \\
(2) & baba & \to & bb & \quad & (5) & baa & \to & bb \\
(3) & bba & \to & bb & \quad & (6) & aabab & \to & bb \\
\end{array}
\right.
$$

# фазз тестирование

система $\tau'_{1}$ завершима и локально конфлюэнтна => по лемме ньюмана конфлюэнтна. Выбор цепочки переписывания для достижения НФ произвольный. так что функцию поиска НФ можно взять из алгоритма Кнута-Бендикса.


Порядок действий:

- генерируем строку и цепочку
- получаем новую строку, применив цепочку в $\tau_{1}$
- в $\tau'_{1}$ приводим обе строки к их НФ и сравниваем НФ.

## мета тестирование

инварианты:
1)наличие хотя бы одной буквы b
2)в случае отсутствия буквы b длина строки сохраняется

const
    MAXN = 10000000;
var
    numbers :array[0..MAXN] of longint;
    i :longint;
begin
    assign(input,'data.txt');
    reset(input);
    for i:=0 to MAXN do
        read(numbers[i]);
end.

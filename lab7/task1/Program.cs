using System.Text;

Console.OutputEncoding = Encoding.UTF8;
Console.InputEncoding = Encoding.UTF8;

Console.WriteLine("Введите путь к текстовому файлу:");
var filePath = Console.ReadLine();

if (string.IsNullOrWhiteSpace(filePath) || !File.Exists(filePath))
{
    Console.WriteLine("Укажите корректный путь к существующему текстовому файлу.");
    return;
}

try
{
    var content = await File.ReadAllTextAsync(filePath, Encoding.UTF8);
    Console.WriteLine("Введите символы для удаления:");
    var charsToRemove = Console.ReadLine(); // абвгдеёжзийклмнопрстуфхцчшщъыьэю

    if (string.IsNullOrWhiteSpace(charsToRemove) || string.IsNullOrWhiteSpace(content))
    {
        Console.WriteLine("Пустой файл или символы для удаления.");
        return;
    }

    var result = new StringBuilder();
    foreach (var c in content)
        if (!charsToRemove.Contains(c))
            result.Append(c);

    await File.WriteAllTextAsync(filePath, result.ToString(), Encoding.UTF8);
    Console.WriteLine("Обновлённое содержимое сохранено.");
}
catch (Exception ex)
{
    Console.WriteLine($"Ошибка: {ex.Message}");
}
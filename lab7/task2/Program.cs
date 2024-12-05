using System.Diagnostics;

async Task<TimeSpan> DownloadImagesInParallel(string apiUrl, int numberOfImages)
{
    var httpClient = new HttpClient();
    var tasks = new List<Task>();

    var stopwatch = Stopwatch.StartNew();

    for (var i = 0; i < numberOfImages; i++) tasks.Add(DownloadImage(httpClient, apiUrl, i));

    await Task.WhenAll(tasks);

    stopwatch.Stop();
    return stopwatch.Elapsed;
}

async Task<TimeSpan> DownloadImagesSequentially(string apiUrl, int numberOfImages)
{
    var httpClient = new HttpClient();

    var stopwatch = Stopwatch.StartNew();

    for (var i = 0; i < numberOfImages; i++) await DownloadImage(httpClient, apiUrl, i);

    stopwatch.Stop();
    return stopwatch.Elapsed;
}

async Task DownloadImage(HttpClient httpClient, string apiUrl, int imageIndex)
{
    try
    {
        var response = await httpClient.GetAsync(apiUrl);
        if (response.IsSuccessStatusCode)
        {
            var jsonResponse = await response.Content.ReadAsStringAsync();
            Console.WriteLine($"Image {imageIndex + 1} downloaded successfully: {jsonResponse}");
        }
        else
        {
            Console.WriteLine($"Failed to download image {imageIndex + 1}. Status code: {response.StatusCode}");
        }
    }
    catch (Exception ex)
    {
        Console.WriteLine($"Exception occurred while downloading image {imageIndex + 1}: {ex.Message}");
    }
}

async Task Main()
{
    const int numberOfImages = 20;
    var apiUrl = "https://dog.ceo/api/breeds/image/random";

    Console.WriteLine("Starting parallel download...");
    var parallelTime = await DownloadImagesInParallel(apiUrl, numberOfImages);
    Console.WriteLine($"Parallel download completed in {parallelTime.TotalSeconds} seconds.\n");

    Console.WriteLine("Starting sequential download...");
    var sequentialTime = await DownloadImagesSequentially(apiUrl, numberOfImages);
    Console.WriteLine($"Sequential download completed in {sequentialTime.TotalSeconds} seconds.");
}

await Main();
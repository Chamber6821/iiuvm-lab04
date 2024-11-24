#include <chrono>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <windows.h>

#include <setupapi.h>
#include <winreg.h>

std::string timestamp() {
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  return std::format("{}", ms.count());
}

void printCameraInfo(cv::VideoCapture &cap) {
  std::uint32_t fourcc = cap.get(cv::CAP_PROP_FOURCC);
  std::string fourccStr = {char(fourcc), char(fourcc >> 8), char(fourcc >> 16),
                           char(fourcc >> 24)};

  std::cout << "\n=== Camera Information ===\n";
  std::cout << "Resolution: " << cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x"
            << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
  std::cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << std::endl;
  std::cout << "Backend: " << cap.getBackendName() << std::endl;
  std::cout << "Auto Exposure: " << cap.get(cv::CAP_PROP_AUTO_EXPOSURE)
            << std::endl;
  std::cout << "Brightness: " << cap.get(cv::CAP_PROP_BRIGHTNESS) << std::endl;
  std::cout << "Contrast: " << cap.get(cv::CAP_PROP_CONTRAST) << std::endl;
  std::cout << "Saturation: " << cap.get(cv::CAP_PROP_SATURATION) << std::endl;
  std::cout << "FOURCC: " << fourccStr << std::endl;
  std::cout << "Built-in Microphone: "
            << (cap.get(cv::CAP_PROP_AUDIO_BASE_INDEX) >= 0 ? "Yes" : "No")
            << std::endl;
  std::cout << "=========================\n\n";
}

void recordVideo(cv::VideoCapture &cap) {
  int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  std::string filename = "video_" + timestamp() + ".avi";

  cv::VideoWriter video(filename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                        30, cv::Size(frame_width, frame_height));

  cv::Mat frame;
  cv::namedWindow("Video Recording", cv::WINDOW_AUTOSIZE);

  std::cout << "Video recording started. Press 'q' to stop." << std::endl;

  while (true) {
    cap.read(frame);
    if (frame.empty())
      break;

    video.write(frame);
    cv::imshow("Video Recording", frame);

    char c = (char)cv::waitKey(1);
    if (c == 'q' || c == 'Q')
      break;
  }

  video.release();
  cv::destroyAllWindows();
  std::cout << "Video saved as: " << filename << std::endl;
}

void takePhoto(cv::VideoCapture &cap, bool hidden = false) {
  cv::Mat frame;

  if (hidden) {
    if (cap.read(frame) && !frame.empty()) {
      std::string filename = "hidden_photo_" + timestamp() + ".jpg";
      cv::imwrite(filename, frame);
      std::cout << "Hidden photo saved as: " << filename << std::endl;
    }
    return;
  }

  cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
  std::cout << "Press 'space' for photo or 'q' to exit" << std::endl;

  while (true) {
    cap.read(frame);
    if (frame.empty())
      break;

    cv::imshow("Camera", frame);
    char key = (char)cv::waitKey(1);

    if (key == ' ') {
      std::string filename = "photo_" + timestamp() + ".jpg";
      cv::imwrite(filename, frame);
      std::cout << "Photo saved as: " << filename << std::endl;
    } else if (key == 'q' || key == 'Q') {
      break;
    }
  }

  cv::destroyAllWindows();
}

int main() {
  std::vector<cv::VideoCapture> caps;
  for (int i = 0;; i++) {
    cv::VideoCapture cap(i + cv::CAP_DSHOW);
    if (not cap.isOpened())
      break;
    caps.emplace_back(std::move(cap));
  }

  if (caps.empty()) {
    std::cout << "Error: Could not open camera" << std::endl;
    return -1;
  }

  for (auto cap : caps)
    printCameraInfo(cap);

  while (true) {
    std::cout << "\nSelect operation mode:\n";
    std::cout << "1. Record Video\n";
    std::cout << "2. Take Photo\n";
    std::cout << "3. Hidden Photo\n";
    std::cout << "4. Camera Information\n";
    std::cout << "0. Exit\n";
    std::cout << "Your choice: ";

    int choice;
    std::cin >> choice;
    if (choice == 0)
      break;

    int index = 1;
    if (caps.size() > 1) {
      std::cout << std::format("Select camera (1 - {}): ", caps.size());
      std::cin >> index;
      if (not(1 <= index and index <= caps.size())) {
        std::cout << "Invalid index\n";
        continue;
      }
    }
    auto &cap = caps[index - 1];

    switch (choice) {
    case 1: {
      recordVideo(cap);
      break;
    }
    case 2:
      takePhoto(cap, false);
      break;
    case 3:
      ShowWindow(GetConsoleWindow(), SW_HIDE);
      takePhoto(cap, true);
      break;
    case 4:
      printCameraInfo(cap);
      break;
    default:
      std::cout << "Invalid choice!\n";
    }
  }

  return 0;
}

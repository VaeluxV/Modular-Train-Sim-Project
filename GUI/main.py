import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk
import os

# Function to switch between frames
def show_frame(frame):
    frame.tkraise()

# Button press action
def button_pressed(msg):
    print(msg)

# Close the secondary screen
def close_secondary_screen():
    show_frame(main_frame)

# Main app window
root = tk.Tk()
root.title("Train Sim Controller Management Interface")
root.iconphoto(False, tk.PhotoImage(file=os.path.join("images", "logo.png")))
root.geometry("600x450")
root.minsize(500, 450)  # Minimum window size

# Apply a manual dark mode style
bg_color = "#2e2e2e"
fg_color = "#ffffff"
button_bg = "#444444"
button_fg = "#ffffff"
entry_bg = "#3c3c3c"
entry_fg = "#ffffff"
frame_bg = "#2e2e2e"

root.configure(bg=bg_color)

# Configure grid layout
root.rowconfigure(0, weight=1)
root.columnconfigure(0, weight=1)

# Main frame (first screen)
main_frame = tk.Frame(root, bg=frame_bg)
main_frame.grid(row=0, column=0, sticky="nsew")

# Secondary frame (scrollable screen)
secondary_frame = tk.Frame(root, bg=frame_bg)
secondary_frame.grid(row=0, column=0, sticky="nsew")

# Scrollable canvas inside the secondary frame
canvas = tk.Canvas(secondary_frame, bg=frame_bg, highlightthickness=0)
scrollbar = ttk.Scrollbar(secondary_frame, orient="vertical", command=canvas.yview)
scroll_frame = tk.Frame(canvas, bg=frame_bg)

scroll_frame.bind(
    "<Configure>",
    lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
)

canvas.create_window((0, 0), window=scroll_frame, anchor="nw")
canvas.configure(yscrollcommand=scrollbar.set)

# Place canvas and scrollbar
canvas.pack(side="left", fill="both", expand=True)
scrollbar.pack(side="right", fill="y")

# Widgets for main frame
main_title = tk.Label(main_frame, text="Main Control Panel", font=("Helvetica", 18), bg=bg_color, fg=fg_color)
main_title.pack(pady=20)

input_label = tk.Label(main_frame, text="Input Field", bg=bg_color, fg=fg_color)
input_label.pack(pady=10)

input_field = tk.Entry(main_frame, bg=entry_bg, fg=entry_fg, insertbackground=entry_fg, width=30)
input_field.pack(pady=10)

# Buttons on the main frame
button_1 = tk.Button(main_frame, text="Button 1", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Button 1 Pressed"))
button_1.pack(pady=5)

button_2 = tk.Button(main_frame, text="Button 2", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Button 2 Pressed"))
button_2.pack(pady=5)

button_open_secondary = tk.Button(main_frame, text="Open Secondary Screen", bg=button_bg, fg=button_fg, command=lambda: show_frame(secondary_frame))
button_open_secondary.pack(pady=5)

# Widgets for the scrollable secondary frame
secondary_title = tk.Label(scroll_frame, text="Advanced Settings", font=("Helvetica", 18), bg=bg_color, fg=fg_color)
secondary_title.grid(row=0, column=0, columnspan=3, pady=20)

# Placeholder image display
image_label = tk.Label(scroll_frame, bg=frame_bg)
image_label.grid(row=1, column=0, columnspan=3, pady=10)

# Load and resize the placeholder image from 'images' directory
image_path = os.path.join("images", "placeholder.png")
image = Image.open(image_path)
image = image.resize((150, 150), Image.Resampling.LANCZOS)  # Resize to fit (keeping the aspect ratio)
image_tk = ImageTk.PhotoImage(image)
image_label.config(image=image_tk)

# Category 1: Toggles
category_1_label = tk.Label(scroll_frame, text="Category 1: Toggles", font=("Helvetica", 14), bg=bg_color, fg=fg_color)
category_1_label.grid(row=2, column=0, columnspan=3, pady=10, sticky="w")

toggle_1 = ttk.Checkbutton(scroll_frame, text="Toggle 1")
toggle_1.grid(row=3, column=0, padx=10, pady=5)

toggle_2 = ttk.Checkbutton(scroll_frame, text="Toggle 2")
toggle_2.grid(row=3, column=1, padx=10, pady=5)

toggle_3 = ttk.Checkbutton(scroll_frame, text="Toggle 3")
toggle_3.grid(row=3, column=2, padx=10, pady=5)

# Category 2: Actions
category_2_label = tk.Label(scroll_frame, text="Category 2: Actions", font=("Helvetica", 14), bg=bg_color, fg=fg_color)
category_2_label.grid(row=4, column=0, columnspan=3, pady=10, sticky="w")

secondary_button_1 = tk.Button(scroll_frame, text="Action 1", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 1 Pressed"))
secondary_button_1.grid(row=5, column=0, padx=10, pady=5)

secondary_button_2 = tk.Button(scroll_frame, text="Action 2", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 2 Pressed"))
secondary_button_2.grid(row=5, column=1, padx=10, pady=5)

secondary_button_3 = tk.Button(scroll_frame, text="Action 3", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 3 Pressed"))
secondary_button_3.grid(row=5, column=2, padx=10, pady=5)

secondary_button_4 = tk.Button(scroll_frame, text="Action 4", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 4 Pressed"))
secondary_button_4.grid(row=6, column=0, padx=10, pady=5)

secondary_button_5 = tk.Button(scroll_frame, text="Action 5", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 5 Pressed"))
secondary_button_5.grid(row=6, column=1, padx=10, pady=5)

secondary_button_6 = tk.Button(scroll_frame, text="Action 6", bg=button_bg, fg=button_fg, command=lambda: button_pressed("Action 6 Pressed"))
secondary_button_6.grid(row=6, column=2, padx=10, pady=5)

# Close button for secondary frame
button_close_secondary = tk.Button(scroll_frame, text="Close", bg=button_bg, fg=button_fg, command=close_secondary_screen)
button_close_secondary.grid(row=7, column=0, columnspan=3, pady=20)

# Initially show the main frame
show_frame(main_frame)

root.mainloop()

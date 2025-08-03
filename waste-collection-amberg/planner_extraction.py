import easyocr
from PIL import Image, ImageEnhance
import numpy as np
import pandas as pd
from pdf2image import convert_from_path
import os

# Load and crop the pdf
def load_pdf_image(pdf_name, box_coords):
    file_path = os.path.join("resources", "pdf-waste-collection-plans", pdf_name)

    images = convert_from_path(file_path, dpi=300)
    img = images[0]
    cropped = img.crop(box_coords)
    return cropped

# Image preprocessing: grayscale + contrast
def preprocess_image(image, threshold=90, contrast_factor=2.5):
    gray = image.convert("L")
    enhancer = ImageEnhance.Contrast(gray)
    enhanced = enhancer.enhance(contrast_factor)
    bw = enhanced.point(lambda x: 0 if x < threshold else 255, '1')
    return bw.convert("RGB")  # back to RGB for OCR

# Extract the columns/rows and run OCR on those cells
def extract_cells(image, months, rows=31, cols=6, lang=['de', 'en']):
    width, height = image.size
    column_width = width // cols
    row_height = height // rows

    reader = easyocr.Reader(lang, gpu=False)
    entries = []

    for col in range(cols):
        for row in range(rows):
            left = col * column_width
            right = (col + 1) * column_width
            top = row * row_height
            bottom = (row + 1) * row_height

            cell_img = image.crop((left, top, right, bottom))
            # Increase image scale for better small character extraction
            scale_factor = 2
            cell_upscaled = cell_img.resize(
                (cell_img.width * scale_factor, cell_img.height * scale_factor),
                resample=Image.BICUBIC
            )
            cell_np = np.array(cell_upscaled)

            if cell_np.size == 0:
                continue

            cell_text = reader.readtext(cell_np, detail=0)
            joined_text = " ".join(cell_text).strip().split()

            entries.append({
                "Month": months[col],
                "Day": row + 1,
                "Text": joined_text
            })

    return entries

# Run full extraction process
def run_extraction(pdf_name, box_coords, csv_name, months=None):
    if months is None:
        months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun"]

    print(f"📄 Loading PDF: {pdf_name}")
    img = load_pdf_image(pdf_name, box_coords)

    print("🧪 Preprocessing...")
    processed = preprocess_image(img)

    print("🔍 OCR per Cell...")
    entries = extract_cells(processed, months)

    print(f"💾 Saved as: {csv_name}")
    df = pd.DataFrame(entries)

    file_path = os.path.join("resources", "ocr-results", csv_name)
    df.to_csv(file_path, index=False)
    return df

# IMPORTANT: 
# Set the box coordinates
# Set top left corner BELOW the month tiles
# Fit box SNUGLY around all 6 month columns

'''
Run this after getting the new pdf calenders 
Change year to year of calender
'''
# Jan - Jun
run_extraction(
    pdf_name="Abfuhrplan_Januar_bis_Juni.pdf",
    box_coords=(135, 320, 3375, 2255),
    csv_name="waste-collection-01_06_2025.csv",
    months=["Jan", "Feb", "Mar", "Apr", "May", "Jun"]
)

# Jul - Dec
run_extraction(
    pdf_name="Abfuhrplan_Juli_bis_Dezember.pdf",
    box_coords=(122, 320, 3385, 2258),
    csv_name="waste-collection-07_12_2025.csv",
    months=["Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
)

#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryEditor
    @brief drop-in memory editor by @ocornut

    https://gist.github.com/ocornut/0673e37e54aff644298b#file-imgui_memory_editor-h
*/
#include "imgui.h"

// Mini memory editor for ImGui (to embed in your game/tools)
// v0.10
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9028162/3047ef88-392c-11e5-8270-a54f8354b208.gif
//
// You can adjust the keyboard repeat delay/rate in ImGuiIO.
// The code assume a mono-space font for simplicity! If you don't use the default font, use ImGui::PushFont()/PopFont() to switch to a mono-space font before caling this.
//
// Usage:
//   static MemoryEditor memory_editor;                                                     // save your state somewhere
//   memory_editor.Draw("Memory Editor", mem_block, mem_block_size, (size_t)mem_block);     // run
//
// TODO: better resizing policy (ImGui doesn't have flexible window resizing constraints yet)
#include <stdio.h>

namespace yakc {

struct MemoryEditor
{
    typedef unsigned char ubyte;
    typedef unsigned short uword;

    typedef ubyte (*cb_read)(void* userdata, uword addr);
    typedef void (*cb_write)(void* userdata, uword addr, ubyte value);

    bool    Open;
    bool    AllowEdits;
    int     Rows;
    int     DataEditingAddr;
    bool    DataEditingTakeFocus;
    char    DataInput[32];
    char    MemAddrInput[32];
    char    MemSizeInput[32];
    uword   MemAddr;
    uword   MemSize;

    MemoryEditor()
    {
        Open = true;
        Rows = 16;
        DataEditingAddr = -1;
        DataEditingTakeFocus = false;
        AllowEdits = true;
        MemAddr = 0x0000;
        MemSize = 0x1000;
        sprintf(MemAddrInput, "%04X", MemAddr);
        sprintf(MemSizeInput, "%04X", MemSize);
    }

    bool Draw(const char* title, cb_read read_func, cb_write write_func, void* userdata)
    {
        if (ImGui::Begin(title, &Open, ImGuiWindowFlags_ShowBorders))
        {
            ImGui::BeginChild("##scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

            float glyph_width = ImGui::CalcTextSize("F").x;
            float cell_width = glyph_width * 3; // "FF " we include trailing space in the width to easily catch clicks everywhere

            float line_height = ImGui::GetTextLineHeight();
            int line_total_count = (int)((MemSize + Rows-1) / Rows);
            ImGuiListClipper clipper(line_total_count, line_height);
            int visible_start_addr = clipper.DisplayStart * Rows;
            int visible_end_addr = clipper.DisplayEnd * Rows;

            bool data_next = false;

            if (!AllowEdits || DataEditingAddr >= MemSize)
                DataEditingAddr = -1;

            int data_editing_addr_backup = DataEditingAddr;
            if (DataEditingAddr != -1)
            {
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && DataEditingAddr >= Rows)                   { DataEditingAddr -= Rows; DataEditingTakeFocus = true; }
                else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && DataEditingAddr < MemSize - Rows)  { DataEditingAddr += Rows; DataEditingTakeFocus = true; }
                else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && DataEditingAddr > 0)                { DataEditingAddr -= 1; DataEditingTakeFocus = true; }
                else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && DataEditingAddr < MemSize - 1)    { DataEditingAddr += 1; DataEditingTakeFocus = true; }
            }
            if ((DataEditingAddr / Rows) != (data_editing_addr_backup / Rows))
            {
                // Track cursor movements
                float scroll_offset = ((DataEditingAddr / Rows) - (data_editing_addr_backup / Rows)) * line_height;
                bool scroll_desired = (scroll_offset < 0.0f && DataEditingAddr < visible_start_addr + Rows*2) || (scroll_offset > 0.0f && DataEditingAddr > visible_end_addr - Rows*2);
                if (scroll_desired)
                    ImGui::SetScrollY(ImGui::GetScrollY() + scroll_offset);
            }

            bool draw_separator = true;
            for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) // display only visible items
            {
                int offset = line_i * Rows;
                ImGui::Text("%04X: ", MemAddr+offset);
                ImGui::SameLine();

                // Draw Hexadecimal
                float line_start_x = ImGui::GetCursorPosX();
                for (int n = 0; n < Rows && offset < MemSize; n++, offset++)
                {
                    ImGui::SameLine(line_start_x + cell_width * n);

                    if (DataEditingAddr == offset)
                    {
                        // Display text input on current byte
                        ImGui::PushID(offset);
                        struct FuncHolder
                        {
                            // FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious.
                            static int Callback(ImGuiTextEditCallbackData* data)
                            {
                                int* p_cursor_pos = (int*)data->UserData;
                                if (!data->HasSelection())
                                    *p_cursor_pos = data->CursorPos;
                                return 0;
                            }
                        };
                        int cursor_pos = -1;
                        bool data_write = false;
                        if (DataEditingTakeFocus)
                        {
                            ImGui::SetKeyboardFocusHere();
                            sprintf(DataInput, "%02X", read_func(userdata, MemAddr+offset));
                        }
                        ImGui::PushItemWidth(ImGui::CalcTextSize("FF").x);
                        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal|ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_AutoSelectAll|ImGuiInputTextFlags_NoHorizontalScroll|ImGuiInputTextFlags_AlwaysInsertMode|ImGuiInputTextFlags_CallbackAlways;
                        if (ImGui::InputText("##data", DataInput, 32, flags, FuncHolder::Callback, &cursor_pos))
                            data_write = data_next = true;
                        else if (!DataEditingTakeFocus && !ImGui::IsItemActive())
                            DataEditingAddr = -1;
                        DataEditingTakeFocus = false;
                        ImGui::PopItemWidth();
                        if (cursor_pos >= 2)
                        {
                            data_write = data_next = true;
                        }
                        if (data_write)
                        {
                            int data;
                            if (sscanf(DataInput, "%X", &data) == 1)
                            {
                                write_func(userdata, MemAddr+offset, (unsigned char)data);
                            }
                        }
                        ImGui::PopID();
                    }
                    else
                    {
                        ImGui::Text("%02X ", read_func(userdata, MemAddr+offset));
                        if (AllowEdits && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
                        {
                            DataEditingTakeFocus = true;
                            DataEditingAddr = offset;
                        }
                    }
                }

                ImGui::SameLine(line_start_x + cell_width * Rows + glyph_width * 2);

                if (draw_separator)
                {
                    ImVec2 screen_pos = ImGui::GetCursorScreenPos();
                    ImGui::GetWindowDrawList()->AddLine(ImVec2(screen_pos.x - glyph_width, screen_pos.y - 9999), ImVec2(screen_pos.x - glyph_width, screen_pos.y + 9999), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));
                    draw_separator = false;
                }

                // Draw ASCII values
                offset = line_i * Rows;
                for (int n = 0; n < Rows && offset < MemSize; n++, offset++)
                {
                    if (n > 0) ImGui::SameLine();
                    int c = read_func(userdata, MemAddr + offset);
                    ImGui::Text("%c", (c >= 32 && c < 128) ? c : '.');
                }
            }
            clipper.End();
            ImGui::PopStyleVar(2);
            
            ImGui::EndChild();

            if (data_next && DataEditingAddr < MemSize)
            {
                DataEditingAddr = DataEditingAddr + 1;
                DataEditingTakeFocus = true;
            }

            ImGui::Separator();

            ImGui::AlignFirstTextHeightToWidgets();
            ImGui::PushItemWidth(56);
            ImGui::PushAllowKeyboardFocus(false);
            int rows_backup = Rows;
            if (ImGui::DragInt("##rows", &Rows, 0.2f, 4, 32, "%.0f rows"))
            {
                ImVec2 new_window_size = ImGui::GetWindowSize();
                new_window_size.x += (Rows - rows_backup) * (cell_width + glyph_width);
                ImGui::SetWindowSize(new_window_size);
            }
            ImGui::PopAllowKeyboardFocus();
            ImGui::PopItemWidth();
            ImGui::SameLine(96);
            ImGui::PushItemWidth(38);
            if (ImGui::InputText("Start", MemAddrInput, 32, ImGuiInputTextFlags_CharsHexadecimal))
            {
                int new_addr = MemAddr;
                if (sscanf(MemAddrInput, "%X", &new_addr) == 1)
                {
                    MemAddr = (uword) new_addr;
                    sprintf(MemAddrInput, "%04X", MemAddr);
                }
            }
            ImGui::SameLine();
            if (ImGui::InputText("Length", MemSizeInput, 32, ImGuiInputTextFlags_CharsHexadecimal))
            {
                int new_size = MemSize;
                if (sscanf(MemSizeInput, "%X", &new_size) == 1)
                {
                    MemSize = (uword) new_size;
                    sprintf(MemSizeInput, "%04X", MemSize);
                }
            }

            ImGui::PopItemWidth();
        }
        ImGui::End();
        return this->Open;
    }
};

} // namespace yakc

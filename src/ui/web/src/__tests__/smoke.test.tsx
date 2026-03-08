import { render, screen } from '@testing-library/react'
import { describe, it, expect } from 'vitest'
import App from '../App'

describe('Smoke test', () => {
  it('should render the app', () => {
    render(<App />)
    expect(screen.getByText(/MeOS Frontend/i)).toBeInTheDocument()
  })
})
